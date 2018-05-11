#pragma once

#include "EngineInterface.h"
#include "DrawTable.h"
#include "Evaluation.h"
#include "EngineInterface.h"
#include "StopWatch.h"
#include "MoveGenerator.h"

enum SEARCHTYPE
{
	NORMAL_SEARCH=1,
	PONDER_SEARCH,
	NODES_SEARCH,
	MATE_SEARCH,
	DEPTH_SEARCH,
	TIME_SEARCH
};

class Engine
{
	friend void EngineSearchThreadLoop(void* eng);
public:
	DWORD strength; // strength in % * 100;
	MoveList bestMove;
	Evaluation eval;
	StopWatch watch;
	MoveGenerator mgen;
	SEARCHTYPE searchtype;
	bool debug;
	DWORD fixedTime;
	DWORD maxTime;
	DWORD fixedNodes;
	DWORD fixedMate;
	DWORD fixedDepth;
	DWORD nodes;
	MoveList searchmoves;
	DrawTable drawTable;
	EngineInterface* ei;
	int contempt;
	ChessBoard theBoard;
	Engine();
	void startSearch();
	void interativeSearch();
	int aspirationSearch(int depth);
	int rootSearch(int depth, int alpha, int beta);
	int Search(int depth, int alpha, int beta, int ply);
	int qSearch(int alpha, int beta, int ply);
	bool abortCheck();
	void sendBestMove();
};