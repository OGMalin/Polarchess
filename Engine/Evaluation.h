#pragma once
#include <assert.h>
#include "ChessBoard.h"
#include "defs.h"
#include "MoveList.h"

const int MAX_EVAL = 100;
class Evaluation;

typedef void (Evaluation::* evalFunction) (const ChessBoard&);

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
	int bishopPair;
	int gamestage;
	bool isEndgame;
	int pieces[32][2];
	typeSquare kingsquare[2];
	PIECELIST pawnlist[2];
	PIECELIST knightlist[2];
	PIECELIST bishoplist[2];
	PIECELIST rooklist[2];
	PIECELIST queenlist[2];
	int position[2]; // The score
	int pawnscore[2];
	evalFunction fMiddleGame[MAX_EVAL];
	evalFunction fEndGame[MAX_EVAL];
	evalFunction fPawnMiddleGame[MAX_EVAL];
	evalFunction fPawnEndGame[MAX_EVAL];
	Evaluation();
	void setup(const ChessBoard& cb);
	void addEval(evalFunction f, bool middle, bool end);
	void addPawnEval(evalFunction f, bool middle, bool end);
	int evaluate(const ChessBoard& cb, int alpha, int beta);
	void scanBoard(const ChessBoard& cb);
	bool isDraw(const ChessBoard& cb);
	bool cantWin(const ChessBoard& cb);
	bool cantLose(const ChessBoard& cb);
	void evalStatic(const ChessBoard& cb);
	void evalStaticEndgame(const ChessBoard& cb);
	void evalPawnstructure(const ChessBoard& cb);
	void evalBishopPair(const ChessBoard& cb);
};