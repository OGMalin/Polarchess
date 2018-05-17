#pragma once
#include <assert.h>
#include "ChessBoard.h"
#include "defs.h"
#include "MoveList.h"

const int MAX_EVAL = 100;
class Evaluation;

struct PIECELIST
{
	typeSquare square[10];
	int size;
	inline void add(typeSquare sq){square[size++] = sq;};
};
class Evaluation
{
public:
	// Return a score seen from the side to move
	typeColor rootcolor;
	int drawscore[2];
	int pawnValue;
	int knightValue;
	int bishopValue;
	int rookValue;
	int queenValue;
	int pieces[32][2];
	typeSquare kingsquare[2];
	PIECELIST pawnlist[2];
	PIECELIST knightlist[2];
	PIECELIST bishoplist[2];
	PIECELIST rooklist[2];
	PIECELIST queenlist[2];
	int position[2]; // The score
	Evaluation();
	int evaluate(const ChessBoard& cb, int alpha, int beta);
	void scanBoard(const ChessBoard& cb);
	void evalMaterial(const ChessBoard& cb);
	void evalStatic(const ChessBoard& cb);
	void evalStaticEndgame(const ChessBoard& cb);
	bool isDraw(const ChessBoard& cb);
};