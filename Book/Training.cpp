#include "Training.h"
#include <algorithm>
#include <QProgressDialog>
#include <QApplication>

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

ChessMove TrainingPath::currentMove()
{
	return moves[current].move;
};

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

void Training::create(QWidget* parent)
{
	// Read White base
	QVector<BookDBEntry> pos;
	ChessBoard b;
	TrainingPath path;
	TrainingLine tline;
	QVector<TrainingLine> tlines;
	int i, j, rep;
	bool exist;

	int steps=19, step;
	QProgressDialog progress("Creating trainingdata...", "Abort", 0, steps, parent);
	progress.setWindowModality(Qt::WindowModal);
	progress.show();
	QApplication::processEvents();
	step = 0;
	for (rep = 0; rep < 2; rep++)
	{
		// Should it be done for this base?
		if (!Base[rep])
			continue;
		progress.setLabelText("Reading from positions from database ...");
		progress.setValue(++step);
		QApplication::processEvents();
		if (progress.wasCanceled())
			break;
		list.clear();

		// Collect all lines
		Base[rep]->getTrainingPosition(pos);

		if (progress.wasCanceled())
			break;
		if (pos.size() > 0)
		{
			progress.setLabelText("Sorting positions...");
			progress.setValue(++step);
			QApplication::processEvents();
			std::sort(pos.begin(), pos.end());
			progress.setLabelText("Creating lines ...");
			progress.setValue(++step);
			QApplication::processEvents();
			if (progress.wasCanceled())
				break;
			b.setStartposition();
			walkThrough(b, path, 0, pos, rep);
		}

		progress.setLabelText("Tuning lines ...");
		progress.setValue(++step);
		QApplication::processEvents();
		if (progress.wasCanceled())
			break;

		// Remove last move if it not a repertoire move
		for (i = 0; i < list.size(); i++)
		{
			j = list[i].moves.size() % 2;
			if ((rep == WHITE) && (j == 0))
				list[i].moves.pop_back();
			else if ((rep == BLACK) && (j == 1))
				list[i].moves.pop_back();
		}

		progress.setLabelText("Update score ...");
		progress.setValue(++step);
		QApplication::processEvents();
		if (progress.wasCanceled())
			break;

		// Update score
		for (i = 0; i < list.size(); i++)
		{
			if (list[i].moves.size() > 0)
				list[i].score = list[i].moves[list[i].moves.size() - 1].score;
			else
				list[i].score = 0;
		}

		progress.setLabelText("Sorting lines ...");
		progress.setValue(++step);
		QApplication::processEvents();
		if (progress.wasCanceled())
			break;
		// Sort list based on score;
		std::sort(list.begin(), list.end());

		progress.setLabelText("Preparing for saving to database ...");
		progress.setValue(++step);
		QApplication::processEvents();
		if (progress.wasCanceled())
			break;
		// Save the list to db;
		tlines.clear();
		for (i = 0; i < list.size(); i++)
		{
			tline.start = 0;
			tline.score = list[i].score;
			tline.moves.clear();
			b.setStartposition();
			for (j = 0; j < list[i].moves.size(); j++)
			{
				if (j > 0)
					tline.moves += " ";
				tline.moves += b.makeMoveText(list[i].moves[j].move, FIDE).c_str();
				b.doMove(list[i].moves[j].move, false);
			}
			if (!tline.moves.isEmpty())
				tlines.push_back(tline);
		}
		progress.setLabelText("Deleting old lines ...");
		progress.setValue(++step);
		QApplication::processEvents();
		if (progress.wasCanceled())
			break;
		Base[rep]->deleteTrainingLines();
		progress.setLabelText("Saving to database ...");
		progress.setValue(++step);
		QApplication::processEvents();
		if (progress.wasCanceled())
			break;
		Base[rep]->addTrainingLines(tlines);
	}
	progress.setValue(steps);
}

void Training::walkThrough(ChessBoard& cb, TrainingPath& path, int ply, QVector<BookDBEntry>& pos, int color)
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
		tpe.score = bid->score;
		path.moves.push_back(tpe);
		cb.doMove(tpe.move, false);
		walkThrough(cb, path, ply + 1, pos, color);
		path.moves.pop_back();
		// Only first move for repertoire
		if (bde.board.toMove == color)
			break;
		++curmove;
		if (curmove >= bid->movelist.size())
			break;
		cb = bde.board;
	}
}

bool Training::get(TrainingPath& line, int color, ChessBoard& cb)
{
	int i,j;
	bool found;
	ChessBoard b;
	QVector<TrainingPath> tlines;

	getAll(tlines, color);
	if (!cb.isStartposition())
	{
		for (i = 0; i < tlines.size(); i++)
		{
			found = false;
			b.setStartposition();
			for (j = 0; j < tlines[i].moves.size(); j++)
			{
				b.doMove(tlines[i].moves[j].move, false);
				if (cb == b)
				{
					found = true;
					break;
				}
			}
			if (!found)
				tlines.remove(i--);
		}
	}

	std::sort(tlines.begin(), tlines.end());

	if (!tlines.size())
		return false;
	line = tlines.first();
	return true;
}

void Training::getAll(QVector<TrainingPath>& allTP, int color)
{
	int rep, i;
	QVector<TrainingLine> lines;
	TrainingPath tp;
	allTP.clear();
	for (rep = 0; rep < 2; rep++)
	{
		if (!Base[rep])
			continue;
		if (color != -1)
			if (color != rep)
				continue;
		Base[rep]->getTrainingLines(lines);
		for (i = 0; i < lines.size(); i++)
		{
			tp.rowid = lines[i].rowid;
			tp.score = lines[i].score;
			tp.color = rep;
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
	TrainingPathEntry tpe;
	QStringList slist = smoves.split(" ");
	cb.setStartposition();
	tp.moves.clear();
	for (int i = 0; i < slist.size(); i++)
	{
		tp.endposition = cb;
		tpe.move=cb.getMoveFromText(slist[i].toStdString());
		tp.moves.push_back(tpe);
		cb.doMove(tpe.move, false);
	}
}
