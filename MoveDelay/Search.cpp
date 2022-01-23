#include <process.h>
#include "Search.h"
#include "..\Common\Utility.h"
//#include <iostream>

// Use with logging
//#include <fstream>
//std::ofstream logfileS;
//#define LOG(txt) logfileS.open("MoveDelay.log", ios::out | ios::app);logfileS << txt << endl;logfileS.close();
// Else
#define LOG(txt) {};

using namespace std;

CRITICAL_SECTION searchCS;

void Search::stopTimer()
{
	if (!timerID)
		return;
	DeleteTimerQueueTimer(NULL, timerID, NULL);
	timerID = NULL;
}

VOID CALLBACK searchTimerProc(VOID* lpv, BOOLEAN)
{
	Search* search = (Search*)lpv;
	search->stopTimer();
	search->input();
}

void Search::startTimer(int ms)
{
	if (timerID)
		stopTimer();
	CreateTimerQueueTimer(&timerID, NULL, searchTimerProc, this, ms, 0, WT_EXECUTEONLYONCE);
}

Search::Search()
{
	InitializeCriticalSection(&searchCS);
	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	timerID = NULL;
}

Search::~Search()
{
	DeleteCriticalSection(&searchCS);
	CloseHandle(hEvent);

}

bool Search::checkMove(ChessBoard& cb, int wtime, int btime, std::string bestmove, std::string ponder, int delay)
{
	int t;
	if (cb.toMove == WHITE)
		t = wtime;
	else
		t = btime;
	
	// Don't spend time if there is only one legal move
	if (cb.legalMoves() < 2)
		return true;

	// Don't spend time if in timetrouble
	if (t < 5000)
		return true;
	
	// Don't use delay
	if (delay == DELAY_none)
		return true;
	
	// Check for easy move
	LOG("S! " + cb.getFen());
	LOG("S! Check for easy move");
	if (easyMove(cb, cb.getMoveFromText(bestmove)))
		return true;
	LOG("S! No easymove found.");
	switch (delay)
	{
	case DELAY_fast:
		t /= 100;
		break;
	case DELAY_normal:
		t /= 60;
		break;
	case DELAY_slow:
		t /= 40;
		break;
	}

	EnterCriticalSection(&searchCS);
	lastline = "bestmove " + bestmove;
	if (!ponder.empty())
		lastline += " ponder " + ponder;
	LOG("S! Delaying:" + lastline);
	LeaveCriticalSection(&searchCS);
	startTimer(t);
	return false;
}

void Search::input()
{
	EnterCriticalSection(&searchCS);
	LOG("S! Sending:" + lastline);
	inQue.push_back(lastline);
	LeaveCriticalSection(&searchCS);
	SetEvent(hEvent);
}

int Search::get(std::string& s)
{
	int ret = SEARCH_none;
	EnterCriticalSection(&searchCS);
	if (inQue.size() > 0)
	{
		s = inQue.front();
		inQue.pop_front();
		if (getWord(s, 1) == "bestmove")
			ret = SEARCH_bestmove;
		else if (getWord(s, 1) == "info")
			ret = SEARCH_info;
	}
	LeaveCriticalSection(&searchCS);
	return ret;
}

void Search::stop()
{
	stopTimer();
}

bool Search::easyMove(const ChessBoard& startboard, const ChessMove& bm)
{
	ChessBoard cb = startboard;
	int bestscore;
	int score;
	int depth;
	int alpha = -MATE;
	int beta = MATE;
	ml[0].clear();
	mgen.makeMoves(cb, ml[0]);
	if (ml[0].size() == 0)
		return true;

	// Search for the best move
	int mit;
	bestscore = -MATE;
	ChessMove bestmove = ml[0][0];
	for (depth = 1; depth < 4; depth++)
	{
		for (mit = 0; mit < ml[0].size(); mit++)
		{
			mgen.doMove(cb, ml[0][mit]);
			bestscore = -alphaBeta(cb, -beta, -alpha, depth, 1);
			mgen.undoMove(cb, ml[0][mit]);
			if (bestscore >= beta)
			{
				break;
			}
			if (bestscore > alpha)
			{
				bestmove = ml[0][mit];
				alpha = bestscore;
			}
		}
		ml[0].swap(ml[0].find(bestmove), 0);
//		cout << endl;
	}

	// If the move played isn't the best move it isn't an easy move
	if (bm != bestmove)
	{
		LOG("S! Didn't play the best move so no need for more search");
		return false;
	}

	ml[0].erase(bm);
	bestmove = ml[0][0];

	score = -MATE;
	for (depth = 1; depth < 4; depth++)
	{
		for (mit = 0; mit < ml[0].size(); mit++)
		{
			mgen.doMove(cb, ml[0][mit]);
			score = -alphaBeta(cb, -beta, -alpha, depth, 1);
			mgen.undoMove(cb, ml[0][mit]);
			if (score >= beta)
				break;
			if (score > alpha)
			{
				alpha = score;
				bestmove = ml[0][mit];
			}
		}
		ml[0].swap(ml[0].find(bestmove), 0);
	}
	if (bestscore > score + 299)
	{
		LOG("S! Next best score are much lower than best score so this is an easy move.")
		return true;
	}

	return false;
}

int Search::alphaBeta(ChessBoard& cb, int alpha, int beta, int depth, int ply)
{
//	cout << ply << " ";
	int score;

	if (depth == 0)
		return quinceSearch(cb, alpha, beta, ply + 1);

	ml[ply].clear();
	mgen.makeMoves(cb, ml[ply]);
	int mit;
	for (mit = 0; mit < ml[ply].size(); mit++)
	{
		mgen.doMove(cb, ml[ply][mit]);
		score = -alphaBeta(cb, -beta, -alpha, depth-1, ply+1);
		mgen.undoMove(cb, ml[ply][mit]);
		if (score >= beta)
			return beta;
		if (score > alpha)
		{
			alpha = score;
		}
	}
	if (mit == 0)
	{
		if (!cb.inCheck()) // Stalemate
			alpha = 0;
		else
			alpha = -MATE;
	}
	return alpha;
}

int Search::quinceSearch(ChessBoard& cb, int alpha, int beta, int ply)
{
//	cout << ply << " ";
	int score;
	score = eval(cb);
	if (ply >= MAX_PLY)
		return score;

	if (score >= beta)
		return beta;

	if (score > alpha)
		alpha = score;
	ml[ply].clear();
	mgen.makeCaptureMoves(cb, ml[ply]);

	int mit;
	for (mit = 0; mit < ml[ply].size(); mit++)
	{
		mgen.doMove(cb, ml[ply][mit]);
		score = -quinceSearch(cb,-beta, -alpha,ply+1);
		mgen.undoMove(cb, ml[ply][mit]);
		if (score >= beta)
			return beta;
		if (score > alpha)
			alpha = score;
	}
	return alpha;
}

int Search::eval(ChessBoard& cb)
{
	typeSquare sq;
	typePiece piece;
	int pawnValue = 100;
	int knightValue = 300;
	int bishopValue = 300;
	int rookValue = 500;
	int queenValue = 900;
	int score = 0;
	for (sq = 0; sq < 0x88; sq++)
	{
		if (LEGALSQUARE(sq))
		{
			piece = cb.board[sq];
			if (piece)
			{
				switch (piece)
				{
				case whitepawn:
					score += pawnValue;
					break;
				case whiteknight:
					score += knightValue;
					break;
				case whitebishop:
					score += bishopValue;
					break;
				case whiterook:
					score += rookValue;
					break;
				case whitequeen:
					score += queenValue;
					break;
				case blackpawn:
					score -= pawnValue;
					break;
				case blackknight:
					score -= knightValue;
					break;
				case blackbishop:
					score -= bishopValue;
					break;
				case blackrook:
					score -= rookValue;
					break;
				case blackqueen:
					score -= queenValue;
					break;
				}
			}
		}
	}
	return (cb.toMove == WHITE) ? score : -score;
}
