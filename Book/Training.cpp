#include "Training.h"
#include <algorithm>

Training::Training()
{
	Base[0] = Base[1] = NULL;
}

Training::~Training()
{

}

void Training::SetDatabase(int color, Database* base)
{
	Base[color] = base;
}

bool posLess(const BookDBEntry& b1, const BookDBEntry& b2)
{
	return b1.board < b2.board;
}

void Training::create(ChessBoard* cb, int color)
{
	// Read White base
	Database*base;
	QVector<BookDBEntry> pos;

	if (Base[0] && ((color == -1) || (color == 0)))
	{
		base = Base[0];
		base->getTrainingPosition(pos);
		std::sort(pos.begin(), pos.end() , posLess);
	}
}
