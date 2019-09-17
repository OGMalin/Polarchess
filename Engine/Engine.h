#pragma once

#include "EngineInterface.h"
#include "DrawTable.h"
#include "Evaluation.h"
#include "EngineInterface.h"
#include "../Common/StopWatch.h"
#include "../Common/MoveGenerator.h"
#include "../Common/defs.h"

enum SEARCHTYPE
{
	NORMAL_SEARCH=1,
	PONDER_SEARCH,
	NODES_SEARCH,
	MATE_SEARCH,
	DEPTH_SEARCH,
	TIME_SEARCH
};

enum
{
	lowerbound=1,
	upperbound
};

class Engine
{
	friend void EngineSearchThreadLoop(void* eng);
public:
	ChessMove bestMove;
	Evaluation eval;
	StopWatch watch;
	MoveGenerator mgen;
	SEARCHTYPE searchtype;
	MoveList pv[MAX_PLY];
	MoveList ml[MAX_PLY];
	ChessMove nullmove[MAX_PLY];
	bool debug;
	ULONGLONG fixedTime;
	ULONGLONG maxTime;
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
	ChessBoard tempBoard;
	// Keep track of materiale on board to deside if nullmove is dangerous
	int material[13];
	Engine();
	void startSearch();
	void iterativeSearch(bool inCheck, HASHKEY hashKey);
	int aspirationSearch(int depth, int bestscore, bool inCheck, HASHKEY hashKey);
	int rootSearch(int depth, int alpha, int beta, bool inCheck, HASHKEY hashKey);
	int Search(int depth, int alpha, int beta, bool inCheck, HASHKEY hashKey, int ply, bool followPV, bool doNullmovem, ChessMove& lastmove);
	int qSearch(int alpha, int beta, int ply);
	void orderRootMoves();
	// Order movelist, put m as first move.
	void orderMoves(MoveList& mlist, const ChessMove& m);
	void orderQMoves(MoveList& mlist);
	bool abortCheck();
	void sendBestMove();
	// Type=0-> Normal, 1=lowerbound, 2=upperbound
	void sendPV(const MoveList& l, int depth, int score, int type = 0);
	void copyPV(MoveList& m1, MoveList& m2, ChessMove& m);
	int moveExtention(bool inCheck, ChessMove& move, ChessMove& lastmove, int moves);
};
