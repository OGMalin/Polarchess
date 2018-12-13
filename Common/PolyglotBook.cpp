#pragma once

#include "PolyglotBook.h"


Polyglot::Polyglot()
{
	isOpen = false;
}

Polyglot::Polyglot(QString& bookfile)
{
	isOpen = open(bookfile);
}

Polyglot::~Polyglot()
{
	close();
}

bool Polyglot::open(QString& bookfile)
{
	return true;
}

void Polyglot::close()
{
	isOpen = false;
}

void Polyglot::get(ChessBoard& board, QVector<ChessMove>& moves)
{
	moves.clear();
}
