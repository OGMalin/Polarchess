#pragma once

#include <memory.h>
#include <string.h>
#include <ctype.h>
#include "ChessBoard.h"
#include "MoveGenerator.h"
#include "defs.h"

using namespace std;

ChessBoard::ChessBoard()
{ 
	clear();
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
	size_t len, i;

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
		i = len - 1;
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
