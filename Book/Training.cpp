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
	TrainingLine tline;
	QVector<TrainingLine> tlines;
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
				b.setStartposition();
				walkThrough(b, path, 0, pos);
			}
		}

		// Remove lines if starting from a position
		b.setStartposition();
		if (cb != b)
		{
			for (i = 0; i < list.size(); i++)
			{
				exist = false;
				b.setStartposition();
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
			if (list[i].moves.size() > 0)
				list[i].endscore = list[i].moves[list[i].moves.size() - 1].endscore;
			list[i].endscore = 0;
		}

		// Sort list based on endscore;
		std::sort(list.begin(), list.end());

		// Save the list to db;
		for (i = 0; i < list.size(); i++)
		{
			tline.start = 0;
			tline.endscore = list[i].endscore;
			tline.moves.clear();
			b.setStartposition();
			for (j = 0; j < list[i].moves.size(); j++)
			{
				if (b == cb)
					tline.start = j;
				if (j > 0)
					tline.moves += " ";
				tline.moves += b.makeMoveText(list[i].moves[j].move, FIDE).c_str();
				b.doMove(list[i].moves[j].move, false);
			}
			tlines.push_back(tline);
		}
		Base[rep]->addTrainingLine(tlines);
	}
}

void Training::walkThrough(ChessBoard& cb, TrainingPath& path, int ply, QVector<BookDBEntry>& pos)
{
	int  curmove = 0;
	BookDBEntry bde;
	QVector<BookDBEntry>::iterator bid;
	TrainingPathEntry tpe;

	// Get position 
	bde.board = cb;
	bid = std::lower_bound(pos.begin(), pos.end(), bde);

	// The position don't exist or no moves or repeating move
	if ((bid->board != cb) || (bid->movelist.size() == 0) || (bid->dirty))
	{
		list.push_back(path);
		return;
	}
	
	bid->dirty = true;

	while (1)
	{
		tpe.move = bid->movelist[curmove].move;
		tpe.endscore = bid->endscore;
		path.moves.push_back(tpe);
		cb.doMove(tpe.move, false);
		walkThrough(cb, path, ply + 1, pos);
		path.moves.pop_back();
		++curmove;
		if (curmove >= bid->movelist.size())
			break;
		cb = bde.board;
	}
}

bool Training::get(TrainingLine& line)
{
	TrainingLine tline[2];
	int rep;
	for (rep = 0; rep < 2; rep++)
		Base[rep]->getTrainingLine(tline[rep]);
	if (tline[0].moves.isEmpty() && tline[1].moves.isEmpty())
		return false;
	if (tline[0].moves.isEmpty())
	{
		line = tline[1];
		return true;
	}
	if (tline[1].moves.isEmpty())
	{
		line = tline[0];
		return true;
	}

	if (tline[0].endscore <= tline[1].endscore)
	{
		line = tline[0];
		return true;
	}
	line = tline[1];
	return true;

}

