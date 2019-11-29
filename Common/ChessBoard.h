#pragma once

#include <string>
#include "defs.h"
#include "ChessMove.h"

enum { FIDE, SAN, LAN, COOR, UCI };

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
	// Get the fen-string for current position
	char* getFen(char* buffer, bool skipmovecount = false);
	std::string getFen(bool skipmovecount=false);
	bool doMove(ChessMove& m, bool legalcheck);
	bool doMove(const char* sz);
	// Checking from square, to square, castle and promoting (defaults to queen)
	bool isLegal(ChessMove& m);
	// If illegal move the move.score>0
	const ChessMove getMoveFromText(const std::string text);
	// Strip 'non' important characters from a movestring
	char* stripMoveText(char* mt);
	bool isRowChar(char c);
	bool isFileChar(char c);
	bool isPieceChar(char c);
	typePiece getPieceFromChar(char c);
	char getCharFromPiece(typePiece p);
	HASHKEY hashkey();
	HASHKEY newHashkey(const ChessMove& m, HASHKEY oldkey);

	// Type:
	//  FIDE - Fide standard
	//  SAN  - PGN standard
	//  LAN  - Long algebraic Notation
	//  COOR - Coordinate system (Winboard standard)
	//  UCI  - UCI standard
	char* makeMoveText(const ChessMove& cm, char* buf, int bufsize, int type);
	const std::string makeMoveText(const ChessMove& m, int type);
	bool isStartposition();
	void setStartposition();

	// Used when sorting and searching
	friend bool operator<(const ChessBoard& b1, const ChessBoard& b2);
	friend bool operator==(const ChessBoard& b1, const ChessBoard& b2);
	friend bool operator!=(const ChessBoard& b1, const ChessBoard& b2);
};