#include "Evaluation.h"
#include "StaticEval.h"
#include "StaticEndgame.h"

Evaluation::Evaluation()
{
	rootcolor = WHITE;
	// Drawscore are calculated in Engine file (contempt).
	drawscore[0] = 0;
	drawscore[1] = 0;

	// Default evaluation terms.
	pawnValue = 100;
	knightValue = 300;
	bishopValue = 300;
	rookValue = 500;
	queenValue = 900;

}

int Evaluation::evaluate(const ChessBoard& cb, int alpha, int beta)
{
	int score;
	bool endgame = false;
	position[0] = position[1] = 0;
	if (cb.move50draw > 98)
		return drawscore[cb.toMove];
	scanBoard(cb);
	evalMaterial(cb);
	if ((position[WHITE] + position[BLACK]) < 3000)
		endgame = true;

	if (endgame)
		evalStaticEndgame(cb);
	else
		evalStatic(cb);

	score = (cb.toMove == WHITE) ? (position[WHITE] - position[BLACK]) : (position[BLACK] - position[WHITE]);
	return score;
}

void Evaluation::scanBoard(const ChessBoard& cb)
{
	typeSquare sq;
	typePiece piece;
	int i=0,j;
	for (sq = 0; sq < 0x88; sq++)
	{
		if (LEGALSQUARE(sq))
		{
			piece = cb.board[sq];
			if (piece)
			{
				pieces[i][0] = piece;
				pieces[i++][1] = sq;
			}
		}
	}
	for (j = i; j < 32; j++)
		pieces[j][0] = 0;
}

void Evaluation::evalMaterial(const ChessBoard& cb)
{
	int i = 0;
	while (pieces[i][0])
	{
		switch (pieces[i][0])
		{
		case whitepawn:
			position[WHITE] += pawnValue;
			break;
		case whiteknight:
			position[WHITE] += knightValue;
			break;
		case whitebishop:
			position[WHITE] += bishopValue;
			break;
		case whiterook:
			position[WHITE] += rookValue;
			break;
		case whitequeen:
			position[WHITE] += queenValue;
			break;
		case blackpawn:
			position[BLACK] += pawnValue;
			break;
		case blackknight:
			position[BLACK] += knightValue;
			break;
		case blackbishop:
			position[BLACK] += bishopValue;
			break;
		case blackrook:
			position[BLACK] += rookValue;
			break;
		case blackqueen:
			position[BLACK] += queenValue;
			break;
		}
		++i;
		if (i == 32)
			break;
	}
}

void Evaluation::evalStatic(const ChessBoard& cb)
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

void Evaluation::evalStaticEndgame(const ChessBoard& cb)
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
