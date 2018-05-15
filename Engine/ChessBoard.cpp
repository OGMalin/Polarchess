#pragma once

#include <memory.h>
#include <string.h>
#include <ctype.h>
#include "ChessBoard.h"
#include "MoveGenerator.h"
#include "defs.h"
#include "Utility.h"

using namespace std;

static bool initHash = false;

// keyValues to be XOR'ed
// key[0-11][0-63] = Value for each pieces on each square
// key[12][0-15]   = Value for castle rights
// key[12][16]     = Value for black on move.
// key[12][17-24]  = Value for ep file
static HASHKEY ZobristKey[13][64];

ChessBoard::ChessBoard()
{ 
	clear();
}

ChessBoard::ChessBoard(const ChessBoard& cb)
{
	copy(cb);
}

ChessBoard::~ChessBoard()
{
}

ChessBoard& ChessBoard::operator=(const ChessBoard& b)
{
	copy(b);
	return *this;
}

void ChessBoard::copy(const ChessBoard& b)
{
	memcpy(&board, &b.board, 0x88 * sizeof(typePiece));
	castle = b.castle;
	enPassant = b.enPassant;
	toMove = b.toMove;
	move50draw = b.move50draw;
}

void ChessBoard::clear()
{
	typeSquare sq;
	for (sq = 0; sq < 0x88; sq++)
		board[sq] = EMPTY;
	castle = 0;
	enPassant = UNDEF;
	toMove = WHITE;
	move50draw = 0;
}
int ChessBoard::compare(const ChessBoard& b)
{
	if (memcmp(&board, &(b.board), 0x88 * sizeof(typePiece)) != 0) return 1;
	if (castle != b.castle) return 2;
	if (enPassant != b.enPassant) return 3;
	if (toMove != b.toMove) return 4;
	return 0;
}


void ChessBoard::setFen(const char* szFen)
{
	char fen[256];
	strcpy_s(fen, 256, szFen);
	int file = 0, row = 7;
	unsigned char piece;
	char* psz = fen;
	clear();
	// Get pieces (first substring)
	if (!*psz) return;
	while (*psz <= ' ')
	{
		++psz;
		if (!*psz) return;
	}
	while (*psz>' ')
	{
		piece = EMPTY;
		switch (*psz)
		{
		case 'P': piece = whitepawn;   break;
		case 'N': piece = whiteknight; break;
		case 'B': piece = whitebishop; break;
		case 'R': piece = whiterook;   break;
		case 'Q': piece = whitequeen;  break;
		case 'K': piece = whiteking;   break;
		case 'p': piece = blackpawn;   break;
		case 'n': piece = blackknight; break;
		case 'b': piece = blackbishop; break;
		case 'r': piece = blackrook;   break;
		case 'q': piece = blackqueen;  break;
		case 'k': piece = blackking;   break;
		case '.': ++file; break;
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			if ((*(psz + 1) >= '0') && (*(psz + 1) <= '9'))
			{
				file += (*psz - '0') * 10;
				++psz;
				file += (*psz - '0');
			}
			else
			{
				file += *psz - '0';
			}
			break;
		case '/':
			if (file>0)
				file = 8;
			break;
		}
		if (piece != EMPTY)
		{
			board[SQUARE(file, row)] = piece;
			++file;
		};
		++psz;
		if (file>7)
		{
			row -= file / 8;
			file = file % 8;
		}
		if (row < 0)
			break;
	}
	// Who to move
	if (!*psz) return;
	while (*psz <= ' ')
	{
		++psz;
		if (!*psz) return;
	}
	if (tolower(*psz) == 'b')
		toMove = BLACK;
	else
		toMove = WHITE;
	++psz;
	// Castle
	if (!*psz) return;
	while (*psz <= ' ')
	{
		++psz;
		if (!*psz) return;
	}
	while (*psz>' ')
	{
		switch (*psz)
		{
		case 'Q':
			castle |= whitequeensidecastle;
			break;
		case 'K':
			castle |= whitekingsidecastle;
			break;
		case 'q':
			castle |= blackqueensidecastle;
			break;
		case 'k':
			castle |= blackkingsidecastle;
			break;
		}
		psz++;
	}
	// Enpassant
	if (!*psz) return;
	while (*psz <= ' ')
	{
		++psz;
		if (!*psz) return;
	}
	file = 0, row = 0;
	if ((*psz >= 'a') && (*psz <= 'h'))
		file = *psz - 'a';
	++psz;
	if ((*psz >= '1') && (*psz <= '8'))
		row = *psz - '1';
	if (row>0)
		enPassant = SQUARE(file, row);
	else
		enPassant = UNDEF;
}

bool ChessBoard::doMove(ChessMove& m, bool legalcheck)
{
	MoveGenerator gen;
	if (m.moveType&NULL_MOVE)
	{
		gen.doNullMove(*this, m);
		return true;
	}
	if (legalcheck)
		if (!gen.isLegal(*this, m))
			return false;
	gen.doMove(*this, m);
	return true;
}

const ChessMove ChessBoard::getMoveFromText(const std::string text)
{
	int moveit;
	MoveList ml;
	MoveList allmoves;
	MoveGenerator mgen;
	ChessMove m;
	typePiece piece, ppiece = EMPTY;
	char mt[20];
	int fRow = -1, fFile = -1, tRow = -1, tFile = -1;
	size_t len;
	int i;

	if (text.length()>19)
		strcpy_s(mt, 20, text.substr(0, 18).c_str());
	else
		strcpy_s(mt, 20, text.c_str());
	i = 0;
	while (mt[i]) // uniform castle
	{
		if (mt[i] == '0')
			mt[i] = 'O';
		if (mt[i] == 'o')
			mt[i] = 'O';
		i++;
	}
	stripMoveText(mt);
	len = strlen(mt);
	if (len<2)
	{
		m.score = 1;
		return m; // Move is an empty move by default.
	}
	// Castle
	if (strncmp(mt, "OO", 2) == 0)
	{
		fFile = 4;
		if (strncmp(mt, "OOO", 3) == 0)
			tFile = 2;
		else
			tFile = 6;
		if (toMove == WHITE)
		{
			fRow = 0;
			tRow = 0;
		}
		else
		{
			fRow = 7;
			tRow = 7;
		}
		piece = KING;
	}
	else
	{
		piece = getPieceFromChar(mt[0]);
		i = (int)len - 1;
		if (isPieceChar(toupper(mt[i])))
		{
			ppiece = getPieceFromChar(toupper(mt[i]));
			i--;
		}
		while (i >= 0)
		{
			if (isFileChar(mt[i]))
			{
				if (tFile == -1)
					tFile = mt[i] - 'a';
				else
					fFile = mt[i] - 'a';
			}
			if (isRowChar(mt[i]))
			{
				if (tRow == -1)
					tRow = mt[i] - '1';
				else
					fRow = mt[i] - '1';
			}
			i--;
		}
		if ((fFile>7) || (fRow>7) || (tFile>7) || (tRow>7))
			return m;
		if ((piece == EMPTY) && (fFile == -1))
			fFile = tFile;
		if ((piece == PAWN) && (fFile == -1))
			fFile = tFile;
		if ((fFile >= 0) && (fRow >= 0))
			piece = PIECE(board[SQUARE(fFile, fRow)]);
		if (piece == EMPTY)
			piece = PAWN;
	}

	// Force queen promotion if promotion piece is missing.
	if ((piece == PAWN) && ((tRow == 7) || (tRow == 0)) && (ppiece == EMPTY))
		ppiece = (toMove == WHITE) ? whitequeen : blackqueen;

	piece = PIECE(piece);
	piece = (toMove == WHITE) ? piece : piece + 6;
	mgen.makeMoves(*this, allmoves);
	moveit = 0;
	ml.clear();
	while (moveit != allmoves.size)
	{
		m = (allmoves.list[moveit]);
		if (piece == board[m.fromSquare])
		{
			if (fFile >= 0)
			{
				if (fFile != FILE(m.fromSquare))
				{
					++moveit;
					continue;
				}
			}
			if (tFile >= 0)
			{
				if (tFile != FILE(m.toSquare))
				{
					++moveit;
					continue;
				}
			}
			if (fRow >= 0)
			{
				if (fRow != RANK(m.fromSquare))
				{
					++moveit;
					continue;
				}
			}
			if (tRow >= 0)
			{
				if (tRow != RANK(m.toSquare))
				{
					++moveit;
					continue;
				}
			}
			if (ppiece != m.promotePiece)
			{
				++moveit;
				continue;
			}
			ml.push_back(m);
		}
		++moveit;
	};
	if (ml.size == 1)
	{
		m = (ml.list[ml.begin()]);
	}
	else
	{
		m.clear();
		m.score = 2;
	}
	return m;
}

char* ChessBoard::stripMoveText(char* mt)
{
	int i = 0, j = 0;
	while (!isFileChar(mt[i]) && !isPieceChar(mt[i]) && (mt[i] != 'O'))
	{
		if (mt[i] == '\0')
		{
			mt[0] = '\0';
			return mt;
		}
		i++;
	}
	while (mt[i])
	{
		// Xboard protocol is allowing piecechar to be in non capital letter.
		// when promoting piece is assigned. (e7e8q)
		if (isFileChar(mt[i]) || isRowChar(mt[i]) || isPieceChar(toupper(mt[i])) || mt[i] == 'O')
		{
			mt[j] = mt[i];
			j++;
		}
		i++;
	}
	mt[j] = '\0';
	// For a move like exd6 e.p. the last 'e' would still exist in the movetext (ed6e).
	if (j<2)
	{
		mt[0] = '\0';
	}
	else if ((j>2) && (mt[j - 1] == 'e'))
	{
		mt[j - 1] = '\0';
	}
	return mt;
}

bool ChessBoard::isRowChar(char c)
{
	switch (c)
	{
	case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8':
		return true;
	}
	return false;
}

bool ChessBoard::isFileChar(char c)
{
	switch (c)
	{
	case 'a': case 'b': case 'c': case 'd':
	case 'e': case 'f': case 'g': case 'h':
		return true;
	}
	return false;
}

bool ChessBoard::isPieceChar(char c)
{
	switch (c)
	{
	case 'N': case 'B': case 'R': case 'Q': case 'K':
		return true;
	}
	return false;
}

typePiece ChessBoard::getPieceFromChar(char c)
{
	int cl;
	if (toMove == BLACK)
		cl = 6;
	else
		cl = 0;
	switch (c)
	{
	case 'N':
		return cl + KNIGHT;
	case 'B':
		return cl + BISHOP;
	case 'R':
		return cl + ROOK;
	case 'Q':
		return cl + QUEEN;
	case 'K':
		return cl + KING;
	default:
		return EMPTY;
	}
}

HASHKEY ChessBoard::hashkey()
{
	HASHKEY key = (HASHKEY)0;
	if (!initHash)
	{
		int i, j;

		for (i = 0; i<13; i++)
			for (j = 0; j<64; j++)
				ZobristKey[i][j] = rand64();
		initHash = true;
	}

	// Pieces
	typeSquare sq = 0;
	while (1)
	{
		if (board[sq] != EMPTY)
			key ^= ZobristKey[board[sq] - 1][SQUARE64(sq)];
		sq++;
		if (sq & 8)
		{
			sq += 8;
			if (sq>127)
				break;
		}
	}

	// castle could be any values from 0 to 15
	key ^= ZobristKey[12][castle];

	// EnPassant
	if (enPassant != UNDEF)
		key ^= ZobristKey[12][17 + FILE(enPassant)];

	// Set color to move
	if (toMove == BLACK)
		key ^= ZobristKey[12][16];

	return key;
}

HASHKEY ChessBoard::newHashkey(const ChessMove& m, HASHKEY key)
{
	if (m.moveType != NULL_MOVE)
	{
		// Remove captured piece
		if (m.moveType&CAPTURE)
		{
			if (m.moveType&ENPASSANT)
				key ^= ZobristKey[m.capturedpiece - 1][SQUARE64(m.toSquare) + (toMove == WHITE ? -8 : 8)];
			else
				key ^= ZobristKey[board[m.toSquare] - 1][SQUARE64(m.toSquare)];
		}

		// Remove piece
		key ^= ZobristKey[board[m.fromSquare] - 1][SQUARE64(m.fromSquare)];

		// Add piece
		if (m.moveType&PROMOTE)
			key ^= ZobristKey[m.promotePiece - 1][SQUARE64(m.toSquare)];
		else
			key ^= ZobristKey[board[m.fromSquare] - 1][SQUARE64(m.toSquare)];

		// Move the rook on castle
		if (m.moveType&CASTLE)
		{
			switch (m.toSquare)
			{
			case g1:
				key ^= ZobristKey[whiterook - 1][SQUARE64(h1)];
				key ^= ZobristKey[whiterook - 1][SQUARE64(f1)];
				break;
			case g8:
				key ^= ZobristKey[blackrook - 1][SQUARE64(h8)];
				key ^= ZobristKey[blackrook - 1][SQUARE64(f8)];
				break;
			case c1:
				key ^= ZobristKey[whiterook - 1][SQUARE64(a1)];
				key ^= ZobristKey[whiterook - 1][SQUARE64(d1)];
				break;
			case c8:
				key ^= ZobristKey[blackrook - 1][SQUARE64(a8)];
				key ^= ZobristKey[blackrook - 1][SQUARE64(d8)];
				break;
			}
		}

		// Castle rights (doing it a little complicated because we don't have the new board.
		if (castle&whitecastle || castle&blackcastle)
		{
			typeCastle c = -1, d=-1,e;

			switch (board[m.fromSquare])
			{
			case whiterook:
				if ((m.fromSquare == h1) && (castle&whitekingsidecastle))
					c = castle & 0x0e;
				else if ((m.fromSquare == a1) && (castle&whitequeensidecastle))
					c = castle & 0x0d;
				break;
			case whiteking:
				c = castle & 0x0c;
				break;
			case blackrook:
				if ((m.fromSquare == h8) && (castle&blackkingsidecastle))
					c = castle & 0x0b;
				else if ((m.fromSquare == a8) && (castle&blackqueensidecastle))
					c = castle & 0x07;
				break;
			case blackking:
				c = castle & 0x03;
				break;
			}

			// Rook are captured on his original square
			switch (m.toSquare)
			{
			case h1:
				if ((castle&whitekingsidecastle) && (toMove==BLACK))
					d = castle & 0x0e;
				break;
			case a1:
				if ((castle&whitequeensidecastle) && (toMove == BLACK))
					d = castle & 0x0d;
				break;
			case h8:
				if ((castle&blackkingsidecastle) && (toMove == WHITE))
					d = castle & 0x0b;
				break;
			case a8:
				if ((castle&blackqueensidecastle) && (toMove == WHITE))
					d = castle & 0x07;
				break;
			}
			if ((c != -1) || (d != -1))
			{
				if (c == -1)
					e = d;
				else if (d == -1)
					e = c;
				else
					e = c & d;
				key ^= ZobristKey[12][castle];
				key ^= ZobristKey[12][e];
			}
		}
	}

	// Remove old ep.
	if (enPassant != UNDEF)
		key ^= ZobristKey[12][17 + FILE(enPassant)];

	// Add ep
	if (m.moveType&DBLPAWNMOVE)
	{
		if (LEGALSQUARE(m.toSquare-1)&& (board[m.toSquare-1]== COLORPIECE(OTHERPLAYER(toMove), PAWN)))
			key ^= ZobristKey[12][17 + FILE(m.toSquare)];
		else if ((LEGALSQUARE(m.toSquare + 1)) && (board[m.toSquare + 1] == COLORPIECE(OTHERPLAYER(toMove), PAWN)))
			key ^= ZobristKey[12][17 + FILE(m.toSquare)];
	}

	// Change color to move
	key ^= ZobristKey[12][16];

	return key;
}

const std::string ChessBoard::uciMoveText(const ChessMove& m)
{
	// d7d8q
	string s;
	char file[] = "abcdefgh ";
	char rank[] = "12345678 ";
	char piece[] = " pnbrqk";
	s += file[FILE(m.fromSquare)];
	s += rank[RANK(m.fromSquare)];
	s += file[FILE(m.toSquare)];
	s += rank[RANK(m.toSquare)];
	if (m.promotePiece)
		s += piece[PIECE(m.promotePiece)];
	return s;
}
