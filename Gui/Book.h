#pragma once

#include "../Common/PolyglotBook.h"

class Book : public PolyglotBook
{
public:
	Book();
	~Book();
	bool open();
	ChessMove getMove(ChessBoard&);
};