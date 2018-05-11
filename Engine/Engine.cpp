#include <process.h>
#include <string>
#include "Engine.h"
#include "EngineInterface.h"

using namespace std;
const int MATE = 32767;    // Mate value
const int BREAKING = MATE + 400;
const int MAX_DEPTH = 100;

void EngineSearchThreadLoop(void* lpv)
{
	Engine eng;
	ENGINECOMMAND cmd=ENG_none;
	eng.ei = (EngineInterface*)lpv;
	ChessBoard cb;
	EngineGo eg;
	EngineEval ev;
	while (1)
	{
		WaitForSingleObject(eng.ei->hEngine, INFINITE);
		while ((cmd=eng.ei->peekOutQue())!=ENG_none)
		{
			if (cmd == ENG_quit)
				break;
			switch (cmd)
			{
			case ENG_debug:
				eng.debug = true;
				eng.ei->getOutQue();
				break;
			case ENG_nodebug:
				eng.debug = false;
				eng.ei->getOutQue();
				break;
			case ENG_stop: // No need to do anything here because the engine is in waiting state (or should it sent bestmove 0000 ?).
				eng.ei->getOutQue();
				break;
			case ENG_clearhistory:
				eng.ei->getOutQue();
				eng.drawTable.clear();
				break;
			case ENG_history:
				eng.ei->getOutQue(cb);
				eng.drawTable.add(cb);
				break;
			case ENG_ponderhit: // Shouldnt happend here.
				eng.ei->getOutQue();
				break;
			case ENG_clearhash:
				eng.ei->getOutQue();
				break;
			case ENG_go:
				eng.watch.start();
				eng.ei->getOutQue(eg);
				eng.fixedMate = eg.mate;
				eng.fixedNodes = eg.nodes;
				eng.fixedTime = eg.fixedTime;
				eng.maxTime = eg.maxTime;
				eng.fixedDepth = eg.depth;
				eng.searchmoves = eg.searchmoves;
				if (eng.fixedMate)
					eng.searchtype = MATE_SEARCH;
				else if (eng.fixedNodes)
					eng.searchtype = NODES_SEARCH;
				else if (eng.fixedTime)
					eng.searchtype = TIME_SEARCH;
				else if (eng.fixedDepth)
					eng.searchtype = DEPTH_SEARCH;
				else
					eng.searchtype = NORMAL_SEARCH;
				eng.startSearch();
				break;
			case ENG_ponder:
				eng.watch.start();
				eng.ei->getOutQue(eg);
				eng.fixedMate = eg.mate;
				eng.fixedNodes = eg.nodes;
				eng.fixedTime = eg.fixedTime;
				eng.maxTime = eg.maxTime;
				eng.searchmoves = eg.searchmoves;
				if (eng.fixedMate)
					eng.searchtype = MATE_SEARCH;
				else if (eng.fixedNodes)
					eng.searchtype = NODES_SEARCH;
				else if (eng.fixedTime)
					eng.searchtype = TIME_SEARCH;
				else
					eng.searchtype = PONDER_SEARCH;
				eng.startSearch();
				break;
			case ENG_position:
				eng.ei->getOutQue(cb);
				eng.theBoard = cb;
				break;
			case ENG_eval:
				eng.ei->getOutQue(ev);
				if (ev.type == EVAL_contempt)
					eng.contempt = ev.value;
				else if (ev.type == EVAL_pawn)
					eng.eval.pawnValue = ev.value;
				else if (ev.type == EVAL_knight)
					eng.eval.knightValue = ev.value;
				else if (ev.type == EVAL_bishop)
					eng.eval.bishopValue = ev.value;
				else if (ev.type == EVAL_rook)
					eng.eval.rookValue = ev.value;
				else if (ev.type == EVAL_queen)
					eng.eval.queenValue = ev.value;
				break;
			default:
				// Unknown command, remove it.
				eng.ei->getOutQue();
				break;
			}
		}
	}
	_endthread();
};


Engine::Engine()
{
	strength = 10000; // Strength defaults to 100%
	debug = false;
	contempt = 0;
}

void Engine::startSearch()
{
	nodes = 0;
	bestMove.clear();
	eval.rootcolor = theBoard.toMove;
	eval.drawscore[eval.rootcolor] = -contempt;
	eval.drawscore[OTHERPLAYER(eval.rootcolor)] = contempt;
	interativeSearch();
}

void Engine::interativeSearch()
{
	int depth=1;
	for (depth = 1; depth < MAX_DEPTH; depth++)
	{
		if (aspirationSearch(depth) == BREAKING)
			return;
		if (searchtype == DEPTH_SEARCH)
		{
			if (depth == fixedDepth)
			{
				sendBestMove();
				return;
			}
		}
	}
}

int Engine::aspirationSearch(int depth)
{
	int alpha=-MATE;
	int beta=MATE;
	return rootSearch(depth, alpha, beta);
}

int Engine::rootSearch(int depth, int alpha, int beta)
{
	MoveList ml;
	char sz[256];
	int score;
	bool inCheck;
	++nodes;
	inCheck = mgen.inCheck(theBoard, theBoard.toMove);
	mgen.makeMoves(theBoard, ml);
	if (!ml.size)
	{
		if (!inCheck) // Stalemate
			alpha = 0;
		else
			alpha = -MATE;
		ei->sendInQue(ENG_info, string("string No legal moves, aborting search."));
		return alpha;
	}
	// Order moves
//	orderMoves();
	ml.list[0].score = watch.read();
	bestMove.push_back(ml.list[0]);
	int mit;
	for (mit = 0; mit < ml.size; mit++)
	{
		// Send UCI info
		sprintf_s(sz, 256, "currmove %s currmovenumber %i", theBoard.uciMoveText(ml.list[mit]).c_str(), mit+1);
		ei->sendInQue(ENG_info, sz);

		mgen.doMove(theBoard, ml.list[mit]);
		score = -Search(depth - 1, -beta, -alpha, 1);
		if (score == -BREAKING)
			return BREAKING;
		mgen.undoMove(theBoard, ml.list[mit]);
		if (score >= beta)
			return beta;
		if (score > alpha)
		{
			alpha = score;
			ml.list[mit].score = watch.read();
			bestMove.push_back(ml.list[mit]);
		}
	}
	return alpha;
}

int Engine::Search(int depth, int alpha, int beta, int ply)
{
	MoveList ml;
	int score;
	bool inCheck;
	if (depth == 0)
		return qSearch(alpha, beta, ply);

	if (!(++nodes % 0x400))
		if (abortCheck())
			return BREAKING;

	inCheck = mgen.inCheck(theBoard, theBoard.toMove);

	mgen.makeMoves(theBoard, ml);
	int mit;
	for (mit = 0; mit < ml.size; mit++)
	{
		mgen.doMove(theBoard, ml.list[mit]);
		score = -Search(depth - 1, -beta, -alpha, ply+1);
		if (score == -BREAKING)
			return BREAKING;
		mgen.undoMove(theBoard, ml.list[mit]);
		if (score >= beta)
			return beta;
		if (score > alpha)
			alpha = score;
	}
	if (mit == 0)
	{
		if (!inCheck) // Stalemate
			alpha = 0;
		else
			alpha = -MATE + ply;
	}
	return alpha;
}

int Engine::qSearch(int alpha, int beta, int ply)
{
	int score;
	MoveList ml;

	if (!(++nodes % 0x400))
		if (abortCheck())
			return BREAKING;

	score = eval.evaluate(theBoard, alpha, beta);
	if (score >= beta)
		return beta;
	if (score > alpha)
		alpha = score;

	mgen.makeCaptureMoves(theBoard, ml);
	int mit;
	for (mit = 0; mit < ml.size; mit++)
	{
		mgen.doMove(theBoard, ml.list[mit]);
		score = -qSearch(-beta, -alpha, ply+1);
		mgen.undoMove(theBoard, ml.list[mit]);
		if (score >= beta)
			return beta;
		if (score > alpha)
			alpha = score;
	}
	return alpha;
}

bool Engine::abortCheck()
{
	ENGINECOMMAND cmd;
	ChessBoard cb;
	EngineGo eg;
	EngineEval ev;
	switch (searchtype)
	{
	case NODES_SEARCH:
		if (nodes >= fixedNodes)
		{
			sendBestMove();
			return true;
		}
		break;
	case TIME_SEARCH:
		if (watch.read() >= fixedTime)
		{
			sendBestMove();
			return true;
		};
		break;
	case NORMAL_SEARCH:
		if (watch.read() >= maxTime)
		{
			sendBestMove();
			return true;
		}
		break;
	}
	while ((cmd = ei->peekOutQue()) != ENG_none)
	{
		switch (cmd)
		{
		case ENG_quit:
			return true;
		case ENG_debug:
			debug = true;
			ei->getOutQue();
			break;
		case ENG_nodebug:
			debug = false;
			ei->getOutQue();
			break;
		case ENG_stop:
			ei->getOutQue();
			sendBestMove();
			return true;
		case ENG_clearhistory:
			ei->getOutQue();
			drawTable.clear();
			break;
		case ENG_history:
			ei->getOutQue(cb);
			drawTable.add(cb);
			break;
		case ENG_ponderhit:
			ei->getOutQue();
			maxTime+=watch.read();
			searchtype=NORMAL_SEARCH;
			break;
		case ENG_clearhash:
			ei->getOutQue();
			break;
		case ENG_go: // Shouldnt happend here
			ei->getOutQue(eg);
			break;
		case ENG_ponder: // Shouldnt happend here
			ei->getOutQue(eg);
			break;
		case ENG_position:
			ei->getOutQue(cb);
			break;
		case ENG_eval:
			ei->getOutQue(ev);
			if (ev.type == EVAL_contempt)
				contempt = ev.value;
			else if (ev.type == EVAL_pawn)
				eval.pawnValue = ev.value;
			else if (ev.type == EVAL_knight)
				eval.knightValue = ev.value;
			else if (ev.type == EVAL_bishop)
				eval.bishopValue = ev.value;
			else if (ev.type == EVAL_rook)
				eval.rookValue = ev.value;
			else if (ev.type == EVAL_queen)
				eval.queenValue = ev.value;
			break;
		default:
			// Unknown command, remove it.
			ei->getOutQue();
			break;
		}
	}
	return false;
}

void Engine::sendBestMove()
{
	string s;

	// Full strength
	if (strength == 10000)
	{
		ei->sendInQue(ENG_string, "bestmove " + theBoard.uciMoveText(bestMove.back()));
		return;
	}
	DWORD dw = (DWORD)((DOUBLE)watch.read()*((double)strength/10000));
	int mit;
	for (mit = 0; mit < bestMove.size; mit++)
	{
		if ((DWORD)bestMove.list[mit].score > dw)
			break;
	}
	--mit;
	if (mit < 0)
		mit = 0;
	ei->sendInQue(ENG_string, "bestmove " + theBoard.uciMoveText(bestMove.list[mit]));
}