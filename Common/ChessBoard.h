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
	ChessBoard(const ChessBoard& cb);
	virtual ~ChessBoard();
	ChessBoard& operator=(const ChessBoard& b);
	void copy(const ChessBoard& b);
	void clear();
	// Comparing two boards
	// returns: 0 - equal
	//          1 - pieces are different
	//          2 - The castle rights are different
	//          3 - The enpassant square is different
	//          4 - The color to move is different.
	virtual int compare(const ChessBoard& b);
	void setFen(const char* szFen);
	bool doMove(ChessMove& m, bool legalcheck);
	const ChessMove getMoveFromText(const std::string text);
	// Strip 'non' important characters from a movestring
	char* stripMoveText(char* mt);
	bool isRowChar(char c);
	bool isFileChar(char c);
	bool isPieceChar(char c);
	typePiece getPieceFromChar(char c);
	HASHKEY hashkey();
	HASHKEY newHashkey(const ChessMove& m, HASHKEY oldkey);
	const std::string uciMoveText(const ChessMove& m);
};