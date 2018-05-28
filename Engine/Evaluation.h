#pragma once
#include <assert.h>
#include "../Common/ChessBoard.h"
#include "../Common/defs.h"
#include "../Common/MoveList.h"
#include "../Common/MoveGenerator.h"

const int MAX_EVAL = 100;
class Evaluation;

typedef void (Evaluation::* evalFunction) (ChessBoard&);

struct PIECELIST
{
	typeSquare square[10];
	int size;
	inline void add(typeSquare sq){square[size++] = sq;};
};

class Evaluation
{
	MoveList testList;
	ChessMove testMove;
	MoveGenerator testGen;

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
	int mobilityScore;
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
	int mobility[2];
	evalFunction fMiddleGame[MAX_EVAL];
	evalFunction fEndGame[MAX_EVAL];
	evalFunction fPawnMiddleGame[MAX_EVAL];
	evalFunction fPawnEndGame[MAX_EVAL];
	Evaluation();
	void setup(ChessBoard& cb);
	void addEval(evalFunction f, bool middle, bool end);
	void addPawnEval(evalFunction f, bool middle, bool end);
	int evaluate(ChessBoard& cb, int alpha, int beta);
	void scanBoard(ChessBoard& cb);
	bool isDraw(ChessBoard& cb);
	bool cantWin(ChessBoard& cb);
	bool cantLose(ChessBoard& cb);
	void evalStatic(ChessBoard& cb);
	void evalStaticEndgame(ChessBoard& cb);
	void evalPawnstructure(ChessBoard& cb);
	void evalBishopPair(ChessBoard& cb);
	void evalMobility(ChessBoard& cb);
};