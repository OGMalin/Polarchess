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

void Training::create(ChessBoard& cb, int color)
{
	// Read White base
	Database*base;
	QVector<BookDBEntry> pos;
	ChessBoard b;
	TrainingPath path;
	int index;
	list.clear();

	if (Base[WHITE] && ((color == -1) || (color == WHITE)))
	{
		base = Base[WHITE];
		base->getTrainingPosition(pos);
		if (pos.size() > 0)
		{
			std::sort(pos.begin(), pos.end());
			b.startposition();
			walkThrough(b, path, 0, pos);
		}
	}
}

void Training::walkThrough(ChessBoard& cb, TrainingPath& path, int ply, QVector<BookDBEntry>& pos)
{
	int  curmove = 0;
	BookDBEntry bde;
	QVector<BookDBEntry>::iterator bid;

	bde.board = cb;
	bid = std::lower_bound(pos.begin(), pos.end(), bde);
	if (bid->board != cb)
		return;
	while (1)
	{
		if (ply == 0)
		{
			path.start = cb;
		}
		if (bid->movelist.size() < curmove)
			return;
		++curmove;
	}
}