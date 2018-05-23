#include <memory.h>
#include "ChessMove.h"


ChessMove::ChessMove()
{ 
	clear(); 
}

ChessMove::ChessMove(const ChessMove& m)
{
	if (&m != this)
		memcpy(this, &m, sizeof(ChessMove));
}

ChessMove::~ChessMove() 
{
}

ChessMove& ChessMove::operator=(const ChessMove& m)
{
	if (&m != this)
		memcpy(this, &m, sizeof(ChessMove));
	return *this;
}

void ChessMove::clear()
{
	memset(this, 0, sizeof(ChessMove));
	oldEnPassant = UNDEF;
}

bool ChessMove::empty()
{
	if (moveType&NULL_MOVE)
		return false;
	return (fromSquare == toSquare ? true : false);
}

	// Overload of ==
bool operator==(const ChessMove& m1, const ChessMove& m2)
{
	if ((m1.moveType&NULL_MOVE) || (m2.moveType&NULL_MOVE))
	{
		if ((m1.moveType&NULL_MOVE) && (m2.moveType&NULL_MOVE))
			return true;
		return false;
	}
	if (m1.fromSquare == m2.fromSquare)
		if (m1.toSquare == m2.toSquare)
			if (m1.promotePiece == m2.promotePiece)
				return true;
	return false;
};

	// Overload of !=
bool operator!=(const ChessMove& m1, const ChessMove& m2)
{
	if ((m1.moveType&NULL_MOVE) || (m2.moveType&NULL_MOVE))
	{
		if ((m1.moveType&NULL_MOVE) && (m2.moveType&NULL_MOVE))
			return false;
		return true;
	}


	if (m1.fromSquare != m2.fromSquare)
		return true;
	if (m1.toSquare != m2.toSquare)
		return true;
	if (m1.promotePiece != m2.promotePiece)
		return true;
	return false;
};
