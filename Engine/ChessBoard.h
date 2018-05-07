#pragma once

#include <string>
#include "defs.h"
#include "ChessMove.h"

class ChessBoard
{
public:
	typePiece board[0x88];
	typeCastle castle;
	typeSquare enPassant;
	typeColor toMove;
	int move50draw;
	ChessBoard();
	virtual ~ChessBoard();
	ChessBoard& operator=(const ChessBoard& b);
	void copy(const ChessBoard& b);
	void clear();
	void setFen(const char* szFen);
	bool doMove(ChessMove& m, bool legalcheck);
	const ChessMove getMoveFromText(const std::string text);
	// Strip 'non' important characters from a movestring
	char* stripMoveText(char* mt);
	bool isRowChar(char c);
	bool isFileChar(char c);
	bool isPieceChar(char c);
	typePiece getPieceFromChar(char c);
};