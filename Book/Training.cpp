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
	QVector<BookDBEntry> pos;
	ChessBoard b;
	TrainingPath path;
	int i, j, rep;
	bool exist;

	for (rep = 0; rep < 2; rep++)
	{
		list.clear();

		// Collect all lines
		if (Base[rep] && ((color == -1) || (color == rep)))
		{
			Base[rep]->getTrainingPosition(pos);
			if (pos.size() > 0)
			{
				std::sort(pos.begin(), pos.end());
				b.startposition();
				walkThrough(b, path, 0, pos);
			}
		}

		// Remove lines if starting from a position
		b.startposition();
		if (cb != b)
		{
			for (i = 0; i < list.size(); i++)
			{
				exist = false;
				b = list[i].start;
				for (j = 0; j < list[i].moves.size(); j++)
				{
					if (b == cb)
					{
						exist = true;
						break;
					}
					b.doMove(list[i].moves[j].move, false);
				}
				if (!exist)
					list.remove(i--);
			}
		}

		// Remove last move if it not a repertoire move
		for (i = 0; i < list.size(); i++)
		{
			j = list[i].moves.size() % 2;
			if ((rep == WHITE) && (j == 0))
				list[i].moves.pop_back();
			else if ((rep == BLACK) && (j == 1))
				list[i].moves.pop_back();
		}

		// Update score
		for (i = 0; i < list.size(); i++)
		{
			list[i].movescore = 0;
			list[i].endscore = 0;
			for (j = 0; j < list[i].moves.size(); j++)
			{
				list[i].movescore += list[i].moves[j].movescore;
			}
			if (list[i].moves.size() > 0)
				list[i].endscore = list[i].moves[list[i].moves.size() - 1].endscore;
		}

		// Sort list based on endscore and movescore;
		std::sort(list.begin(), list.end());

		// Save the list to db;
//		Base[rep]
	}
}

void Training::walkThrough(ChessBoard& cb, TrainingPath& path, int ply, QVector<BookDBEntry>& pos)
{
	int  curmove = 0;
	BookDBEntry bde;
	QVector<BookDBEntry>::iterator bid;
	TrainingPathEntry tpe;
	bde.board = cb;
	bid = std::lower_bound(pos.begin(), pos.end(), bde);
	if (bid->board != cb)
		return;
	if (bid->movelist.size() < curmove)
		return;
	
	// Stop when repeating moves
	if (bid->dirty)
		return;
	bid->dirty = true;

	while (1)
	{
		if (ply == 0)
		{
			path.start = cb;
		}
		tpe.move = bid->movelist[curmove].move;
		tpe.endscore = bid->endscore;
		tpe.movescore = bid->movescore;
		path.moves.push_back(tpe);
		cb.doMove(tpe.move, false);
		walkThrough(cb, path, ply + 1, pos);
		cb = bde.board;
		++curmove;
		if (bid->movelist.size() < curmove)
			return;
		list.push_back(path);
	}
}

bool operator<(const TrainingPath& t1, const TrainingPath& t2)
{
	if (t1.endscore < t2.endscore)
		return true;
	if (t1.endscore != t2.endscore)
		return false;
	if (t1.movescore < t2.movescore)
		return true;
	return false;
};
