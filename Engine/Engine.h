#pragma once

#include "EngineInterface.h"
#include "DrawTable.h"
#include "Evaluation.h"
#include "EngineInterface.h"
#include "StopWatch.h"
#include "MoveGenerator.h"
#include "defs.h"

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
	MoveList pv[MAX_PLY];
	MoveList ml[MAX_PLY];
	bool debug;
	DWORD fixedTime;
	DWORD maxTime;
	DWORD fixedNodes;
	DWORD fixedMate;
	DWORD fixedDepth;
	DWORD nodes;
	DWORD multiPV;
	MoveList searchmoves;
	DrawTable drawTable;
	HashDrawTable hashDrawTable;
	EngineInterface* ei;
	int contempt;
	ChessBoard theBoard;
	Engine();
	void startSearch();
	void interativeSearch(bool inCheck, HASHKEY hashKey);
	int aspirationSearch(int depth, int bestscore, bool inCheck, HASHKEY hashKey);
	int rootSearch(int depth, int alpha, int beta, bool inCheck, HASHKEY hashKey);
	int Search(int depth, int alpha, int beta, bool inCheck, HASHKEY hashKey, int ply, bool followPV);
	int qSearch(int alpha, int beta, int ply);

	// Order movelist, put m as first move.
	void orderMoves(MoveList& mlist, const ChessMove& m);
	bool abortCheck();
	void sendBestMove();
	void sendPV(const MoveList& l,int depth);
	void copyPV(MoveList& m1, MoveList& m2, ChessMove& m);
};