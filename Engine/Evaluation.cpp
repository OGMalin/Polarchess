#include "Evaluation.h"
#include "StaticEval.h"
#include "StaticEndgame.h"
#include "../Common/MoveGenerator.h"

const int ENDGAME = 3000;

Evaluation::Evaluation()
{
	rootcolor = WHITE;
	// Drawscore are calculated in Engine file (contempt).
	drawscore[0] = 0;
	drawscore[1] = 0;


	// Default evaluation terms.
	pawnValue = 100;
	knightValue = 325;
	bishopValue = 325;
	rookValue = 500;
	queenValue = 950;
	bishopPair = 40;
	mobilityScore = 2;
}

void Evaluation::setup(ChessBoard& cb)
{
	int i;
	for (i = 0; i < MAX_EVAL; i++)
	{
		fMiddleGame[i] = NULL;
		fEndGame[i] = NULL;
		fPawnMiddleGame[i] = NULL;
		fPawnEndGame[i] = NULL;
	}
	scanBoard(cb);

	if (bishopPair && ((bishoplist[WHITE].size > 1) || (bishoplist[BLACK].size > 1)))
		addEval(&Evaluation::evalBishopPair, true, true);
	if (mobilityScore)
		addEval(&Evaluation::evalMobility, true, true);
}

void Evaluation::addEval(evalFunction f, bool middle, bool end)
{
	int i;
	if (middle)
	{
		i = 0;
		while (fMiddleGame[i])
		{
			if (i >= MAX_EVAL)
				return;
			++i;
		}
		fMiddleGame[i] = f;
	}
	if (end)
	{
		i = 0;
		while (fEndGame[i])
		{
			if (i >= MAX_EVAL)
				return;
			++i;
		}
		fEndGame[i] = f;
	}
}

void Evaluation::addPawnEval(evalFunction f, bool middle, bool end)
{
	int i;
	if (middle)
	{
		i = 0;
		while (fPawnMiddleGame[i])
		{
			if (i >= MAX_EVAL)
				return;
			++i;
		}
		fPawnMiddleGame[i] = f;
	}
	if (end)
	{
		i = 0;
		while (fEndGame[i])
		{
			if (i >= MAX_EVAL)
				return;
			++i;
		}
		fPawnEndGame[i] = f;
	}
}

int Evaluation::evaluate(ChessBoard& cb, int alpha, int beta)
{
	int score,i;
	evalFunction efunc;

	if (cb.move50draw > 98)
		return drawscore[cb.toMove];

	scanBoard(cb);

	evalPawnstructure(cb);

	if (isEndgame)
	{
		if (isDraw(cb))
			return drawscore[cb.toMove];
		
		if (evalSpecialEndgame(cb))
		{
			score = (cb.toMove == WHITE) ? (position[WHITE] - position[BLACK]) : (position[BLACK] - position[WHITE]);
			if (score==0)
				return drawscore[cb.toMove];
			return score;
		}
		evalStaticEndgame(cb);

		// try a alphabeta cut
		score = (cb.toMove == WHITE) ? (position[WHITE] - position[BLACK]) : (position[BLACK] - position[WHITE]);
		if ((score < (alpha - 300)) || (score > (beta + 300)))
			return score;


		i = 0;
		while (efunc = fEndGame[i++])
			(this->*efunc)(cb);

		score = (cb.toMove == WHITE) ? (position[WHITE] - position[BLACK]) : (position[BLACK] - position[WHITE]);

		// Correct the score if there isn't any winchances to force 3 fold rep.

		if (cantWin(cb))
		{
			if (position[cb.toMove] > position[OTHERPLAYER(cb.toMove)])
				score = drawscore[cb.toMove] - 1;
		}
		else if (cantLose(cb))
		{
			if (position[cb.toMove] < position[OTHERPLAYER(cb.toMove)])
				score = drawscore[cb.toMove] + 1;
		}
	}
	else
	{
		evalStatic(cb);

		// try a alphabeta cut
		score = (cb.toMove == WHITE) ? (position[WHITE] - position[BLACK]) : (position[BLACK] - position[WHITE]);
		if ((score < (alpha - 300)) || (score >(beta + 300)))
			return score;

		i = 0;
		while (efunc = fMiddleGame[i++])
			(this->*efunc)(cb);
		score = (cb.toMove == WHITE) ? (position[WHITE] - position[BLACK]) : (position[BLACK] - position[WHITE]);
	}


	return score;
}

void Evaluation::scanBoard(ChessBoard& cb)
{
	typeSquare sq;
	typePiece piece;
	int i=0,j;
	pawnlist[WHITE].size = 0;
	pawnlist[BLACK].size = 0;
	knightlist[WHITE].size = 0;
	knightlist[BLACK].size = 0;
	bishoplist[WHITE].size = 0;
	bishoplist[BLACK].size = 0;
	rooklist[WHITE].size = 0;
	rooklist[BLACK].size = 0;
	queenlist[WHITE].size = 0;
	queenlist[BLACK].size = 0;
	position[0] = position[1] = 0;
	gamestage = 0;
	for (sq = 0; sq < 0x88; sq++)
	{
		if (LEGALSQUARE(sq))
		{
			piece = cb.board[sq];
			if (piece)
			{
				pieces[i][0] = piece;
				pieces[i++][1] = sq;
				switch (piece)
				{
				case whitepawn:
					position[WHITE] += pawnValue;
					pawnlist[WHITE].add(sq);
					break;
				case whiteknight:
					position[WHITE] += knightValue;
					knightlist[WHITE].add(sq);
					gamestage += knightValue;
					break;
				case whitebishop:
					position[WHITE] += bishopValue;
					bishoplist[WHITE].add(sq);
					gamestage += bishopValue;
					break;
				case whiterook:
					position[WHITE] += rookValue;
					rooklist[WHITE].add(sq);
					gamestage += rookValue;
					break;
				case whitequeen:
					position[WHITE] += queenValue;
					queenlist[WHITE].add(sq);
					gamestage += queenValue;
					break;
				case whiteking:
					kingsquare[WHITE] = sq;
					break;
				case blackpawn:
					position[BLACK] += pawnValue;
					pawnlist[BLACK].add(sq);
					break;
				case blackknight:
					position[BLACK] += knightValue;
					knightlist[BLACK].add(sq);
					gamestage += knightValue;
					break;
				case blackbishop:
					position[BLACK] += bishopValue;
					bishoplist[BLACK].add(sq);
					gamestage += bishopValue;
					break;
				case blackrook:
					position[BLACK] += rookValue;
					rooklist[BLACK].add(sq);
					gamestage += rookValue;
					break;
				case blackqueen:
					position[BLACK] += queenValue;
					queenlist[BLACK].add(sq);
					gamestage += queenValue;
					break;
				case blackking:
					kingsquare[BLACK] = sq;
					break;
				}
			}
		}
	}
	for (j = i; j < 32; j++)
		pieces[j][0] = 0;

	if (gamestage < ENDGAME)
		isEndgame = true;
	else
		isEndgame = false;
}

void Evaluation::evalStatic(ChessBoard& cb)
{
	int i = 0;
	while (pieces[i][0])
	{
		switch (pieces[i][0])
		{
		case whitepawn:
			position[WHITE] += staticPawnValue[WHITE][pieces[i][1]];
			break;
		case whiteknight:
			position[WHITE] += staticKnightValue[WHITE][pieces[i][1]];
			break;
		case whitebishop:
			position[WHITE] += staticBishopValue[WHITE][pieces[i][1]];
			break;
		case whiterook:
			position[WHITE] += staticRookValue[WHITE][pieces[i][1]];
			break;
		case whitequeen:
			position[WHITE] += staticQueenValue[WHITE][pieces[i][1]];
			break;
		case whiteking:
			position[WHITE] += staticKingValue[WHITE][pieces[i][1]];
			break;
		case blackpawn:
			position[BLACK] += staticPawnValue[BLACK][pieces[i][1]];
			break;
		case blackknight:
			position[BLACK] += staticKnightValue[BLACK][pieces[i][1]];
			break;
		case blackbishop:
			position[BLACK] += staticBishopValue[BLACK][pieces[i][1]];
			break;
		case blackrook:
			position[BLACK] += staticRookValue[BLACK][pieces[i][1]];
			break;
		case blackqueen:
			position[BLACK] += staticQueenValue[BLACK][pieces[i][1]];
			break;
		case blackking:
			position[BLACK] += staticKingValue[BLACK][pieces[i][1]];
			break;
		}
		++i;
		if (i == 32)
			break;
	}

}

void Evaluation::evalStaticEndgame(ChessBoard& cb)
{
	int i = 0;
	while (pieces[i][0])
	{
		switch (pieces[i][0])
		{
		case whitepawn:
			position[WHITE] += staticPawnValue[WHITE][pieces[i][1]];
			break;
		case whiteknight:
			position[WHITE] += staticKnightValue[WHITE][pieces[i][1]];
			break;
		case whitebishop:
			position[WHITE] += staticBishopValue[WHITE][pieces[i][1]];
			break;
		case whiterook:
			//position[WHITE] += staticRookValue[WHITE][pieces[i][1]];
			break;
		case whitequeen:
			//position[WHITE] += staticQueenValue[WHITE][pieces[i][1]];
			break;
		case whiteking:
			position[WHITE] += staticKingEndValue[WHITE][pieces[i][1]];
			break;
		case blackpawn:
			position[BLACK] += staticPawnValue[BLACK][pieces[i][1]];
			break;
		case blackknight:
			position[BLACK] += staticKnightValue[BLACK][pieces[i][1]];
			break;
		case blackbishop:
			position[BLACK] += staticBishopValue[BLACK][pieces[i][1]];
			break;
		case blackrook:
			//position[BLACK] += staticRookValue[BLACK][pieces[i][1]];
			break;
		case blackqueen:
			//position[BLACK] += staticQueenValue[BLACK][pieces[i][1]];
			break;
		case blackking:
			position[BLACK] += staticKingEndValue[BLACK][pieces[i][1]];
			break;
		}
		++i;
		if (i == 32)
			break;
	}

}

bool Evaluation::isDraw(ChessBoard& cb)
{
	int wp, bp;
	// No insufficient material if there are rooks, queens or pawns on the board
	if (queenlist[WHITE].size || queenlist[BLACK].size || rooklist[WHITE].size || rooklist[BLACK].size || pawnlist[WHITE].size || pawnlist[BLACK].size)
		return false;

	wp = knightlist[WHITE].size + bishoplist[WHITE].size;
	bp = knightlist[BLACK].size + bishoplist[BLACK].size;

	// King alone or only one piece is draw
	if ((wp + bp) < 2)
		return true;
	
	// All bishops of same color
	if (!knightlist[WHITE].size && !knightlist[BLACK].size)
	{
		typeColor w = 0, b = 0;
		typeSquare sq = 0;
		for (typeColor c = 0; c < 2; c++)
		{
			while (sq < bishoplist[WHITE].size)
			{
				if (SQUARECOLOR(bishoplist[WHITE].square[sq]) == WHITE)
					++w;
				else
					++b;
				++sq;
			}
		}
		if (!w || !b)
			return true;
	}

	// Only knights are draws.
	if (!bishoplist[WHITE].size && !bishoplist[BLACK].size)
		return true;

	// One bishop or knight on each side draws
	if (((knightlist[WHITE].size + bishoplist[WHITE].size)<2) &&
		((knightlist[BLACK].size + bishoplist[BLACK].size)<2))
		return true;

	// Two knight against bishop or knight can not win
	if ((wp == 2) && (bp == 1))
		if (knightlist[WHITE].size == 2)
			return true;
	if ((wp == 1) && (bp == 2))
		if (knightlist[BLACK].size == 2)
			return true;

	return false;
}

void Evaluation::evalBishopPair(ChessBoard& cb)
{
	if (bishoplist[WHITE].size > 1)
		position[WHITE] += bishopPair;
	if (bishoplist[BLACK].size > 1)
		position[BLACK] += bishopPair;
}

bool Evaluation::cantWin(ChessBoard& cb)
{
	if (cb.toMove == WHITE)
	{
		if (pawnlist[WHITE].size || rooklist[WHITE].size || queenlist[WHITE].size)
			return false;
		if (bishoplist[WHITE].size > 1)
			return false;
		if (bishoplist[WHITE].size && knightlist[WHITE].size)
			return false;
		return true;
	}
	if (pawnlist[BLACK].size || rooklist[BLACK].size || queenlist[BLACK].size)
		return false;
	if (bishoplist[BLACK].size > 1)
		return false;
	if (bishoplist[BLACK].size && knightlist[BLACK].size)
		return false;
	return true;
}

bool Evaluation::cantLose(ChessBoard& cb)
{
	if (cb.toMove == WHITE)
	{
		if (pawnlist[BLACK].size || rooklist[BLACK].size || queenlist[BLACK].size)
			return false;
		if (bishoplist[BLACK].size > 1)
			return false;
		if (bishoplist[BLACK].size && knightlist[BLACK].size)
			return false;
		return true;
	}
	if (pawnlist[WHITE].size || rooklist[WHITE].size || queenlist[WHITE].size)
		return false;
	if (bishoplist[WHITE].size > 1)
		return false;
	if (bishoplist[WHITE].size && knightlist[WHITE].size)
		return false;
	return true;
}

void Evaluation::evalPawnstructure(ChessBoard& cb)
{
	pawnscore[WHITE] = pawnscore[BLACK] = 0;

}

void Evaluation::evalMobility(ChessBoard& cb)
{
	testGen.makeAllMoves(cb, testList);
	mobility[cb.toMove] = testList.size;
	testGen.doNullMove(cb, testMove);
	testGen.makeAllMoves(cb, testList);
	mobility[cb.toMove] = testList.size;
	testGen.undoNullMove(cb, testMove);
	position[WHITE] += mobility[WHITE]*mobilityScore;
	position[BLACK] += mobility[BLACK]*mobilityScore;
}

bool Evaluation::evalSpecialEndgame(ChessBoard& cb)
{
	return false;
}