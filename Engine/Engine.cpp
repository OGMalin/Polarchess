#include <process.h>
#include "Engine.h"
#include "EngineInterface.h"

const int MATE = 32767;    // Mate value
const int BREAKING = MATE + 400;
const int MAX_DEPTH = 100;

void EngineSearchThreadLoop(void* lpv)
{
	Engine eng;
	EngineCommand cmd;
	eng.ei = (EngineInterface*)lpv;
	ChessBoard cb;
	EngineGo eg;

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
				eng.ponder = false;
				break;
			case ENG_clearhash:
				eng.ei->getOutQue();
				break;
			case ENG_go:
				eng.watch.start();
				eng.ei->getOutQue(eg);
				eng.ponder = false;
				eng.fixedMate = eg.mate;
				eng.fixedNodes = eg.nodes;
				eng.fixedTime = eg.fixedTime;
				eng.maxTime = eg.maxTime;
				eng.searchmoves = eg.searchmoves;
				eng.startSearch();
				break;
			case ENG_ponder:
				eng.ei->getOutQue(eg);
				eng.ponder = true;
				eng.fixedMate = eg.mate;
				eng.fixedNodes = eg.nodes;
				eng.fixedTime = eg.fixedTime;
				eng.maxTime = eg.maxTime;
				eng.searchmoves = eg.searchmoves;
				eng.startSearch();
				break;
			case ENG_position:
				eng.ei->getOutQue(cb);
				eng.theBoard = cb;
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
	debug = false;
}

void Engine::startSearch()
{
	interativeSearch();
}

void Engine::interativeSearch()
{
	int depth=1;
	for (depth = 1; depth < MAX_DEPTH; depth++)
	{
		if (aspirationSearch(depth) == BREAKING)
			return;
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
	int score;
	mgen.makeMoves(theBoard, ml);
	int mit;
	for (mit = 0; mit < ml.size; mit++)
	{
		mgen.doMove(theBoard, ml.list[mit]);
		score = -Search(depth - 1, -beta, -alpha, 1);
		if (score == -BREAKING)
			return BREAKING;
		mgen.undoMove(theBoard, ml.list[mit]);
		if (score >= beta)
			return beta;
		if (score > alpha)
			alpha = score;
	}
	return alpha;
}

int Engine::Search(int depth, int alpha, int beta, int ply)
{
	MoveList ml;
	int score;
	if (depth == 0)
		return qSearch(alpha, beta, ply);
	mgen.makeMoves(theBoard, ml);
	int mit;
	for (mit = 0; mit < ml.size; mit++)
	{
		mgen.doMove(theBoard, ml.list[mit]);
		score = -Search(depth - 1, -beta, -alpha, 1);
		if (score == -BREAKING)
			return BREAKING;
		mgen.undoMove(theBoard, ml.list[mit]);
		if (score >= beta)
			return beta;
		if (score > alpha)
			alpha = score;
	}
	return alpha;
}

int Engine::qSearch(int alpha, int beta, int ply)
{
	return 0;
}

bool Engine::abortCheck()
{
	EngineCommand cmd;
	ChessBoard cb;
	EngineGo eg;

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
			// Send BestMove
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
			watch.start();
			ei->getOutQue();
			ponder = false;
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
		default:
			// Unknown command, remove it.
			ei->getOutQue();
			break;
		}
	}
	return false;
}