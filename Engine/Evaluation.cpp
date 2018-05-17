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

	if ((position[WHITE] + position[BLACK]) < 3000)
		endgame = true;

	if (endgame)
	{
		if (isDraw(cb))
			return drawscore[cb.toMove];
		evalStaticEndgame(cb);
	}
	else
	{
		evalStatic(cb);
	}

	score = (cb.toMove == WHITE) ? (position[WHITE] - position[BLACK]) : (position[BLACK] - position[WHITE]);
	return score;
}

void Evaluation::scanBoard(const ChessBoard& cb)
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
					break;
				case whitebishop:
					position[WHITE] += bishopValue;
					bishoplist[WHITE].add(sq);
					break;
				case whiterook:
					position[WHITE] += rookValue;
					rooklist[WHITE].add(sq);
					break;
				case whitequeen:
					position[WHITE] += queenValue;
					queenlist[WHITE].add(sq);
					break;
				case whiteking:
					kingsquare[WHITE] = sq;
					break;
				case blackpawn:
					position[BLACK] += pawnValue;
					pawnlist[WHITE].add(sq);
					break;
				case blackknight:
					position[BLACK] += knightValue;
					knightlist[BLACK].add(sq);
					break;
				case blackbishop:
					position[BLACK] += bishopValue;
					bishoplist[BLACK].add(sq);
					break;
				case blackrook:
					position[BLACK] += rookValue;
					rooklist[BLACK].add(sq);
					break;
				case blackqueen:
					position[BLACK] += queenValue;
					queenlist[BLACK].add(sq);
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

bool Evaluation::isDraw(const ChessBoard& cb)
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