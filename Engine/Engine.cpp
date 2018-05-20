#include <process.h>
#include <string>
#include "Engine.h"
#include "EngineInterface.h"
#include <assert.h>

using namespace std;
const int BREAKING = MATE + 400;
const int MAX_DEPTH = 100;

Engine eng;
char sz[256];
ChessMove emptyMove;

void EngineSearchThreadLoop(void* lpv)
{
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
					eng.eval.pawnValue = (eng.eval.pawnValue*ev.value) / 100;
				else if (ev.type == EVAL_knight)
					eng.eval.knightValue = (eng.eval.knightValue*ev.value) / 100;
				else if (ev.type == EVAL_bishop)
					eng.eval.bishopValue = (eng.eval.bishopValue*ev.value) / 100;
				else if (ev.type == EVAL_rook)
					eng.eval.rookValue = (eng.eval.rookValue*ev.value) / 100;
				else if (ev.type == EVAL_queen)
					eng.eval.queenValue = (eng.eval.queenValue*ev.value) / 100;
				break;
			default:
				// Unknown command, remove it.
				eng.ei->getOutQue();
				break;
			}
		}
		if (cmd == ENG_quit)
			break;
	}
	_endthread();
};


Engine::Engine()
{
	strength = 10000; // Strength defaults to 100%
	debug = false;
	contempt = 0;
	multiPV = 1;
}

void Engine::startSearch()
{
	int i;
	bool inCheck;
	HASHKEY hashKey;
	nodes = 0;
	bestMove.clear();
	eval.rootcolor = theBoard.toMove;
	eval.drawscore[eval.rootcolor] = -contempt;
	eval.drawscore[OTHERPLAYER(eval.rootcolor)] = contempt;
	eval.setup(theBoard);

	inCheck = mgen.inCheck(theBoard, theBoard.toMove);
	hashKey = theBoard.hashkey();

	// Clear pv
	for (i = 0; i<MAX_PLY; i++)
		pv[i].clear();

	if (searchmoves.size)
		ml[0] = searchmoves;
	else
		mgen.makeMoves(theBoard, ml[0]);

	if (!ml[0].size)
	{
		ei->sendInQue(ENG_info, string("string No legal moves, aborting search."));
		return;
	}
	else if ((ml[0].size == 1) && (searchtype == NORMAL_SEARCH))
	{ // Only one legal move
		bestMove.push_back(ml[0].list[0]);
		sendBestMove();
		return;
	}

	interativeSearch(inCheck, hashKey);
}

void Engine::interativeSearch(bool inCheck, HASHKEY hashKey)
{
	int depth=1;
	int score=-MATE;
	int extention = 0;

	if (inCheck)
		++extention;
	
	for (depth = 1; depth < MAX_DEPTH; depth++)
	{
		sprintf_s(sz, 256, "depth %i", depth);
		ei->sendInQue(ENG_info, sz);
		score = aspirationSearch(depth + extention, score, inCheck, hashKey);
		if (score == BREAKING)
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

int Engine::aspirationSearch(int depth, int bestscore, bool inCheck, HASHKEY hashKey)
{
	int alpha;
	int beta;
	int score;
	if (depth > 1)
	{
		alpha = bestscore - 50;
		beta = bestscore + 50;
	}
	else
	{
		alpha = -MATE;
		beta = MATE;
	}
	score = rootSearch(depth, alpha, beta, inCheck, hashKey);
	if (score == BREAKING)
		return BREAKING;
	if ((score <= alpha) || (score >= beta))
	{
		if (score<=alpha)
			pv[0].list[0].score = MATE + 1;
		else
			pv[0].list[0].score = MATE + 2;
		sendPV(pv[0], depth);
		alpha = -MATE;
		beta = MATE;
		score = rootSearch(depth, alpha, beta, inCheck, hashKey);
	}
	return score;
}

int Engine::rootSearch(int depth, int alpha, int beta, bool inCheck, HASHKEY hashKey)
{
	int score;
	int mit;
	HASHKEY newkey;
	bool followPV = true;
	int extention = 0;
	int oldNodes;

	++nodes;

	// Order moves
	if (depth == 1)
	{
		orderRootMoves();
		bestMove.push_back(ml[0].list[0]);
		bestMove.list[bestMove.size - 1].score = watch.read();
	}
	else
	{
		mit = ml[0].find(bestMove.back());
		if (mit < ml[0].size)
			ml[0].list[mit].score = 0x7fffffff;
		ml[0].sort();
//		orderMoves(ml[0], bestMove.back());
	}

	// Add the root position to the drawtable
	hashDrawTable.add(hashKey, 0);

	for (mit = 0; mit < ml[0].size; mit++)
	{
		// Send UCI info
		sprintf_s(sz, 256, "currmove %s currmovenumber %i", theBoard.uciMoveText(ml[0].list[mit]).c_str(), mit+1);
		ei->sendInQue(ENG_info, sz);
		newkey = theBoard.newHashkey(ml[0].list[mit], hashKey);
		mgen.doMove(theBoard, ml[0].list[mit]);

		assert(theBoard.hashkey() == newkey);

		inCheck = mgen.inCheck(theBoard, theBoard.toMove);
		if (inCheck)
			++extention;
		oldNodes = nodes;
		score = -Search(depth - 1 + extention, -beta, -alpha, inCheck, newkey, 1,followPV);
		if (score == -BREAKING)
			return BREAKING;
		mgen.undoMove(theBoard, ml[0].list[mit]);
		ml[0].list[mit].score = nodes - oldNodes;
		if (score >= beta)
			return beta;
		if (score > alpha)
		{
			copyPV(pv[0], pv[1], ml[0].list[mit]);
			pv[0].list[0].score = score;
			sendPV(pv[0], depth);

			alpha = score;

			bestMove.push_back(ml[0].list[mit]);
			bestMove.list[bestMove.size - 1].score = watch.read();
		}
		if (inCheck)
			--extention;
		followPV = false;
	}
	return alpha;
}

int Engine::Search(int depth, int alpha, int beta, bool inCheck, HASHKEY hashKey, int ply, bool followPV)
{
	int score;
	HASHKEY newkey;
	int extention = 0;

//	pv[ply].clear();

	if (depth == 0)
		return qSearch(alpha, beta, ply);


	if (!(++nodes % 0x400))
		if (abortCheck())
			return BREAKING;

	if (drawTable.exist(theBoard, hashKey) || hashDrawTable.exist(hashKey, ply-1))
		return (eval.drawscore[theBoard.toMove]);

	if (ply >= MAX_PLY)
		return eval.evaluate(theBoard, alpha, beta);

	// Add position to the drawtable
	hashDrawTable.add(hashKey, ply);

	mgen.makeMoves(theBoard, ml[ply]);
	orderMoves(ml[ply], followPV?pv[ply].front():emptyMove);
	int mit;
	for (mit = 0; mit < ml[ply].size; mit++)
	{
		newkey = theBoard.newHashkey(ml[ply].list[mit], hashKey);
		mgen.doMove(theBoard, ml[ply].list[mit]);

		assert(theBoard.hashkey() == newkey);

		inCheck = mgen.inCheck(theBoard, theBoard.toMove);
		if (inCheck)
			++extention;
		score = -Search(depth - 1 + extention, -beta, -alpha, inCheck, newkey, ply + 1, followPV);
		if (score == -BREAKING)
			return BREAKING;
		mgen.undoMove(theBoard, ml[ply].list[mit]);
		if (score >= beta)
			return beta;
		if (score > alpha)
		{
			alpha = score;
			copyPV(pv[ply], pv[ply + 1], ml[ply].list[mit]);
		}
		if (inCheck)
			--extention;
		followPV = false;
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

	pv[ply].clear();

	if (!(++nodes % 0x400))
		if (abortCheck())
			return BREAKING;

	score = eval.evaluate(theBoard, alpha, beta);
	if (score >= beta)
		return beta;
	if (ply >= (MAX_PLY-1))
		return score;
	if (score > alpha)
		alpha = score;

	mgen.makeCaptureMoves(theBoard, ml[ply]);
	orderQMoves(ml[ply]);
	int mit;
	for (mit = 0; mit < ml[ply].size; mit++)
	{
		mgen.doMove(theBoard, ml[ply].list[mit]);
		score = -qSearch(-beta, -alpha, ply + 1);
		if (score == -BREAKING)
			return BREAKING;
		mgen.undoMove(theBoard, ml[ply].list[mit]);
		if (score >= beta)
			return beta;
		if (score > alpha)
		{
			alpha = score;
			copyPV(pv[ply], pv[ply + 1], ml[ply].list[mit]);
		}
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
		case ENG_quit: // Let the comman be in the que.
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

	if (searchtype != NORMAL_SEARCH)
	{
		ei->sendInQue(ENG_string, "bestmove " + theBoard.uciMoveText(bestMove.back()));
		return;
	}
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

void Engine::sendPV(const MoveList& pvline, int depth)
{
	string s="";
	int i=0;
	DWORD t = watch.read();
	while (i < pvline.size)
	{ 
		s+=theBoard.uciMoveText(pvline.list[i]);
		++i;
		if (i < pvline.size)
			s += " ";
	}
	if (pvline.list[0].score == MATE + 1)
		sprintf_s(sz, 256, "depth %i nps %u score lowerbound nodes %u time %u pv %s", depth, (DWORD)(nodes / (double)(t / 1000)), nodes, t, s.c_str());
	else if (pvline.list[0].score == MATE + 2)
		sprintf_s(sz, 256, "depth %i nps %u score upperbound nodes %u time %u pv %s", depth, (DWORD)(nodes / (double)(t / 1000)), nodes, t, s.c_str());
	else if (pvline.list[0].score > MATE - 200)
		sprintf_s(sz, 256, "depth %i nps %u score mate %i nodes %u time %u pv %s", depth, (DWORD)(nodes / (double)(t / 1000)), (MATE - pvline.list[0].score)/2+1, nodes, t, s.c_str());
	else if (pvline.list[0].score < -MATE + 200)
		sprintf_s(sz, 256, "depth %i nps %u score mate %i nodes %u time %u pv %s", depth, (DWORD)(nodes / (double)(t / 1000)), (MATE + pvline.list[0].score)/2, nodes, t, s.c_str());
	else
		sprintf_s(sz, 256, "depth %i nps %u score cp %i nodes %u time %u pv %s", depth, (DWORD)(nodes / (double)(t / 1000)), pvline.list[0].score, nodes, t, s.c_str());
	ei->sendInQue(ENG_info, sz);
}

void Engine::orderRootMoves()
{
	int mit;
	for (mit = 0; mit < ml[0].size; mit++)
	{
		mgen.doMove(theBoard, ml[0].list[mit]);
		ml[0].list[mit].score = -eval.evaluate(theBoard, MATE, -MATE);
		mgen.undoMove(theBoard, ml[0].list[mit]);
	}
	ml[0].sort();

}

void Engine::orderMoves(MoveList& mlist, const ChessMove& first)
{
	static int seevalue[13][13] = { // [victem][attacker]
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // No capture
		 0,  6,  5,  4,  3,  2,  1,  6,  5,  4,  3,  2,  1,
		 0, 12, 11, 10,  9,  8,  7, 12, 11, 10,  9,  8,  7,
		 0, 18, 17, 16, 15, 14, 13, 18, 17, 16, 15, 14, 13,
		 0, 24, 23, 22, 21, 20, 19, 24, 23, 22, 21, 20, 19,
		 0, 30, 29, 28, 27, 26, 25, 30, 29, 28, 27, 26, 25,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // King can't be a victem
		 0,  6,  5,  4,  3,  2,  1,  6,  5,  4,  3,  2,  1,
		 0, 12, 11, 10,  9,  8,  7, 12, 11, 10,  9,  8,  7,
		 0, 18, 17, 16, 15, 14, 13, 18, 17, 16, 15, 14, 13,
		 0, 24, 23, 22, 21, 20, 19, 24, 23, 22, 21, 20, 19,
		 0, 30, 29, 28, 27, 26, 25, 30, 29, 28, 27, 26, 25,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 };  // King can't be a victem

	static int promotevalue[13] = { 0,0,1,1,5,30,0,0,1,1,5,30,0 };
	int mit,score;

	if (mlist.size < 2)
		return;
	
	for (mit = 0; mit < mlist.size; mit++)
	{
		mlist.list[mit].score = 0;
		if (mlist.list[mit].moveType & (CAPTURE | PROMOTE))
		{
			score = 0;
			if (mlist.list[mit].moveType&CAPTURE)
				score += seevalue[mlist.list[mit].capturedpiece][theBoard.board[mlist.list[mit].fromSquare]];
			if (mlist.list[mit].moveType&PROMOTE)
				score += promotevalue[mlist.list[mit].promotePiece];
			mlist.list[mit].score = score;
		}
		else if (mlist.list[mit].moveType & CASTLE)
		{
			mlist.list[mit].score = 1;
		}
	}

	mit = mlist.find(first);
	if (mit < mlist.size)
		mlist.list[mit].score = 1000;
	mlist.sort();
}

void Engine::orderQMoves(MoveList& mlist)
{
	static int seevalue[13][13] = { // [victem][attacker]
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // No capture
		0,  6,  5,  4,  3,  2,  1,  6,  5,  4,  3,  2,  1,
		0, 12, 11, 10,  9,  8,  7, 12, 11, 10,  9,  8,  7,
		0, 18, 17, 16, 15, 14, 13, 18, 17, 16, 15, 14, 13,
		0, 24, 23, 22, 21, 20, 19, 24, 23, 22, 21, 20, 19,
		0, 30, 29, 28, 27, 26, 25, 30, 29, 28, 27, 26, 25,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // King can't be a victem
		0,  6,  5,  4,  3,  2,  1,  6,  5,  4,  3,  2,  1,
		0, 12, 11, 10,  9,  8,  7, 12, 11, 10,  9,  8,  7,
		0, 18, 17, 16, 15, 14, 13, 18, 17, 16, 15, 14, 13,
		0, 24, 23, 22, 21, 20, 19, 24, 23, 22, 21, 20, 19,
		0, 30, 29, 28, 27, 26, 25, 30, 29, 28, 27, 26, 25,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 };  // King can't be a victem

	static int promotevalue[13] = { 0,0,1,1,5,30,0,0,1,1,5,30,0 };
	int mit, score;

	if (mlist.size < 2)
		return;

	for (mit = 0; mit < mlist.size; mit++)
	{
		mlist.list[mit].score = 0;
		score = 0;
		if (mlist.list[mit].moveType&CAPTURE)
			score += seevalue[mlist.list[mit].capturedpiece][theBoard.board[mlist.list[mit].fromSquare]];
		if (mlist.list[mit].moveType&PROMOTE)
			score += promotevalue[mlist.list[mit].promotePiece];
		mlist.list[mit].score = score;
	}

	mlist.sort();
}

void Engine::copyPV(MoveList& m1, MoveList& m2, ChessMove& m)
{
	m1.clear();
	m1.push_back(m);
	for (int i = 0; i<m2.size; i++)
		m1.push_back(m2.list[i]);
};
