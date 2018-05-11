#pragma once
#include "ChessBoard.h"
#include "defs.h"

const int MAX_EVAL = 100;
class Evaluation;

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
	int position[2]; // The score
	Evaluation();
	int evaluate(const ChessBoard& cb, int alpha, int beta);
	void scanBoard(const ChessBoard& cb);
	void evalMaterial(const ChessBoard& cb);
	void evalStatic(const ChessBoard& cb);
	void evalStaticEndgame(const ChessBoard& cb);
};