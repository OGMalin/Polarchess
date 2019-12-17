#include "Training.h"
#include <algorithm>

bool TrainingPath::isCorrect(ChessMove& move)
{
	if (moves[current].move == move)
		return true;
	return false;
}

bool TrainingPath::nextMove(ChessMove& move)
{
	if ((current+2) >= moves.size())
		return false;

	move = moves[current + 1].move;
	current+=2;
	return true;
}

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
			else
				list[i].endscore = 0;
		}

		// Sort list based on endscore;
		std::sort(list.begin(), list.end());

		// Save the list to db;
		tlines.clear();
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
			if (!tline.moves.isEmpty())
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

bool Training::get(TrainingPath& line)
{
	TrainingLine tline[2];
	int rep;
	line.clear();
	for (rep = 0; rep < 2; rep++)
		Base[rep]->getTrainingLine(tline[rep]);
	if (tline[WHITE].moves.isEmpty() && tline[BLACK].moves.isEmpty())
		return false;
	line.color = BLACK;
	if (tline[WHITE].moves.isEmpty())
		line.color = BLACK;
	if (tline[BLACK].moves.isEmpty())
		line.color = WHITE;
	if (!tline[WHITE].moves.isEmpty() && !tline[BLACK].moves.isEmpty())
		line.color = (tline[WHITE].endscore <= tline[BLACK].endscore) ? WHITE : BLACK;
	line.endscore = tline[line.color].endscore;
	line.start = tline[line.color].start;
	line.rowid = tline[line.color].rowid;
	convertMoves(tline[line.color].moves, line);
	return true;

}

void Training::getAll(QVector<TrainingPath>& allTP)
{
	int rep, i;
	QVector<TrainingLine> lines;
	TrainingPath tp;
	allTP.clear();
	for (rep = 0; rep < 2; rep++)
	{
		Base[rep]->getTrainingLines(lines);
		for (i = 0; i < lines.size(); i++)
		{
			tp.endscore = lines[i].endscore;
			tp.start = lines[i].start;
			convertMoves(lines[i].moves, tp);
			allTP.push_back(tp);
		}
	}
}

void Training::updateScore(int color, ChessBoard& cb, int rowid, int score)
{
	Base[color]->updateTrainingScore(cb, rowid, score);
}

void Training::convertMoves(const QString& smoves, TrainingPath& tp)
{
	ChessBoard cb;
	ChessMove move;
	TrainingPathEntry tpe;
	QStringList slist = smoves.split(" ");
	cb.setStartposition();
	tp.moves.clear();
	for (int i = 0; i < slist.size(); i++)
	{
		tp.endposition = cb;
		tpe.move=cb.getMoveFromText(slist[i].toStdString());
		tp.moves.push_back(tpe);
		cb.doMove(move, false);
	}
}
