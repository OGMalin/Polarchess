#pragma once

// A simple engine to find out how difficult a move is.

#include <Windows.h>
#include "..\Common\ChessBoard.h"
#include "..\Common\MoveGenerator.h"
#include "..\Common\StopWatch.h"
#include <string>
#include <list>

enum {
	SEARCH_none = 0,
	SEARCH_bestmove,
	SEARCH_info
};

enum {
	DELAY_none = 0,
	DELAY_fast,
	DELAY_normal,
	DELAY_slow,
};

#define MAX_PLY 50

class Search
{
	MoveGenerator mgen;
	MoveList ml[MAX_PLY];
	int alphaBeta(ChessBoard& cb, int alpha, int beta, int depth, int ply);
	int quinceSearch(ChessBoard& cb, int alpha, int beta, int ply);
	int eval(ChessBoard& cb);
public:
	bool easyMove(const ChessBoard& cb, const ChessMove& m);
	std::list<std::string> inQue;
	std::string lastline;
	HANDLE hEvent;
	Search();
	~Search();
	bool checkMove(ChessBoard& cb, int wtime, int btime, std::string bestmove, std::string ponder, int delay, StopWatch& watch);
	void input();
	int get(std::string& s);
	void stop();
	HANDLE timerID;
	void startTimer(int ms);
	void stopTimer();
};
