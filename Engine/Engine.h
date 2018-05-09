#pragma once

#include "EngineInterface.h"
#include "DrawTable.h"
#include "Evaluation.h"
#include "EngineInterface.h"
#include "StopWatch.h"
#include "MoveGenerator.h"

class Engine
{
	friend void EngineSearchThreadLoop(void* eng);
public:
	Evaluation eval;
	StopWatch watch;
	MoveGenerator mgen;
	bool debug;
	bool ponder;
	DWORD fixedTime;
	DWORD maxTime;
	DWORD fixedNodes;
	DWORD fixedMate;
	MoveList searchmoves;
	DrawTable drawTable;
	EngineInterface* ei;
	ChessBoard theBoard;
	Engine();
	void startSearch();
	void interativeSearch();
	int aspirationSearch(int depth);
	int rootSearch(int depth, int alpha, int beta);
	int Search(int depth, int alpha, int beta, int ply);
	int qSearch(int alpha, int beta, int ply);
	bool abortCheck();
};