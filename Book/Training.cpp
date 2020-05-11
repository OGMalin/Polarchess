#include "Training.h"
#include <algorithm>
#include <QProgressDialog>
#include <QApplication>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include "../Common/WinFile.h"

Training::Training()
{
	stat.inBase = 0;
	stat.current = 0;
	startColor = -1;
	startBoard.setStartposition();
	//currentBoard.clear();
	//currentColor = 0;
	Base[0] = Base[1] = NULL;
}

Training::~Training()
{
}

void Training::SetRepertoireDatabase(int color, Database* base)
{
	Base[color] = base;
}

void Training::walkThrough(ChessBoard& cb, TrainingLine& line, int ply, QVector<BookDBEntry>& pos, int color)
{
	int  curmove = 0;
	BookDBEntry bde;
	QVector<BookDBEntry>::iterator bid;
	TrainingMove tm;

	// Get position 
	bde.board = cb;
	bid = std::lower_bound(pos.begin(), pos.end(), bde);

	// The position don't exist or no moves or repeating move
	if ((bid->board != cb) || (bid->movelist.size() == 0) || line.positionExist(cb))
	{
		line.color = color;
		currentLines.push_back(line);
		return;
	}

	bid->dirty = true;

	while (1)
	{
		tm.move = bid->movelist[curmove].move;
		tm.score = bid->score;
		tm.attempt = bid->attempt;
		line.moves.push_back(tm);
		cb.doMove(tm.move, false);
		walkThrough(cb, line, ply + 1, pos, color);
		line.moves.pop_back();
		// Only first move for repertoire
		if (bde.board.toMove == color)
			break;
		++curmove;
		if (curmove >= bid->movelist.size())
			break;
		cb = bde.board;
	}
}

//bool Training::getNext(TrainingDBEntry& line, int color, ChessBoard& cb)
//{
//	int i,j,score;
//	TrainingDBEntry tde;
//	QSqlDatabase db = QSqlDatabase::database(TRAINING);
//	if (!db.open())
//		return false;
//	QSqlQuery query(db);
//	
//	list.clear();
//	// Get all traininglines from training db. If none are found try to create new lines from repertoire db.
//	if (cb.isStartposition())
//	{
//		if (color >= 0)
//		{
//			query.prepare("SELECT rowid, * FROM training WHERE color=:color ORDER BY score;");
//			query.bindValue(":color", color);
//			query.exec();
//			if (!query.next())
//			{
//				createLines(NULL);
//				query.prepare("SELECT rowid, * FROM training WHERE color=:color ORDER BY score;");
//				query.bindValue(":color", color);
//				query.exec();
//				if (!query.next())
//					return false;
//			}
//		}
//		else
//		{
//			query.exec("SELECT rowid, * FROM training ORDER BY score;");
//			if (!query.next())
//			{
//				createLines(NULL);
//				query.exec("SELECT rowid, * FROM training ORDER BY score;");
//				if (!query.next())
//					return false;
//			}
//		}
//		
//		bool first = true;
//		while (1)
//		{
//			tde.rowid = query.value("rowid").toInt();
//			tde.color = query.value("color").toInt();
//			tde.score = query.value("score").toInt();
//			tde.MovesFromString(query.value("moves").toString());
//			if (first)
//				score = tde.score;
//			else if (tde.score > score)
//				break;
//			list.push_back(tde);
//			if (!query.next())
//				break;
//			first = false;
//		}
//	}
//	else
//	{
//		if (color >= 0)
//		{
//			query.prepare("SELECT rowid, * FROM training WHERE color=:color ORDER BY score;");
//			query.bindValue(":color", color);
//			query.exec();
//			if (!query.next())
//			{
//				createLines(NULL);
//				query.prepare("SELECT rowid, * FROM training WHERE color=:color ORDER BY score;");
//				query.bindValue(":color", color);
//				query.exec();
//				if (!query.next())
//					return false;
//			}
//		}
//		else
//		{
//			// Have to search all positions here
//			query.exec("SELECT rowid, * FROM training ORDER BY score;");
//			if (!query.next())
//			{
//				createLines(NULL);
//				query.exec("SELECT rowid, * FROM training ORDER BY score;");
//				if (!query.next())
//					return false;
//			}
//		}
//		while (1)
//		{
//			tde.rowid = query.value("rowid").toInt();
//			tde.color = query.value("color").toInt();
//			tde.score = query.value("score").toInt();
//			tde.MovesFromString(query.value("moves").toString());
//			list.push_back(tde);
//			if (!query.next())
//				break;
//		}
//		bool found;
//		ChessBoard b;
//		for (i = 0; i < list.size(); i++)
//		{
//			found = false;
//			b.setStartposition();
//			for (j = 0; j < list[i].moves.size(); j++)
//			{
//				if (b == cb)
//				{
//					found = true;
//					break;
//				}
//				b.doMove(list[i].moves[j].move, false);
//			}
//			if (!found)
//			{
//				list.remove(i);
//				--i;
//			}
//		}
//		if (!list.size())
//			return false;
//		score = list[0].score;
//		for (i = 1; i < list.size(); i++)
//		{
//			if (list[i].score > score)
//			{
//				list.remove(i, list.size() - i);
//				break;
//			}
//		}
//	}
//	stat.current = list.size();
//
//	if (!list.size())
//		return false;
//
//	// Select a random line from the lines with lowest score
//	srand(time(NULL));
//	i = rand() % list.size();
//
//	line = list[i];
//	line.clearScore();
//	return true;
//}

void Training::updateScore(TrainingLine& tl)
{
	int i;
	ChessBoard cb;

	// Don't save score/attempt in position before start of training position.
	for (i = 0; i < tl.moves.size(); i++)
	{
		if (cb == startBoard)
			break;
		tl.moves[i].score = 0;
		tl.moves[i].attempt = 0;
	}
	Base[tl.color]->updateTrainingScore(tl);
}

void Training::clearAll()
{
	int i;
	for (i = 0; i < 2; i++)
		Base[i]->clearAllTrainingData();

	currentLines.clear();
}

TrainingStatistics Training::getStat()
{
	stat.current = currentLines.size();
	return stat;
}

void Training::getEndpositions(QVector<ChessBoard>& pos, int color)
{
	//ChessBoard cb;
	//TrainingDBEntry tde;
	//int i, j;
	//QSqlDatabase db = QSqlDatabase::database(TRAINING);
	//if (!db.open())
	//	return;
	//QSqlQuery query(db);

	//query.prepare("SELECT moves FROM training WHERE color=:color;");
	//query.bindValue(":color", color);
	//query.exec();
	//if (!query.next())
	//{
	//	createLines(NULL);
	//	query.prepare("SELECT moves FROM training WHERE color=:color;");
	//	query.bindValue(":color", color);
	//	query.exec();
	//	if (!query.next())
	//		return;
	//}

	//while (1)
	//{
	//	tde.MovesFromString(query.value("moves").toString());
	//	pos.push_back(tde.endPosition());
	//	if (!query.next())
	//		break;
	//}
	//// Remove doubles
	//for (i = 0; i < (pos.size()-1); i++)
	//{
	//	for (j = (i + 1); j < pos.size(); j++)
	//	{
	//		if (pos[i] == pos[j])
	//			pos.remove(j);
	//	}
	//}
}

bool Training::getNextLine(TrainingLine& line)
{
	if (currentLines.size() < 1)
		return false;

	// Select a random line from the lines with lowest score
	srand(time(NULL));
	int i = rand() % currentLines.size();
	
	line = currentLines[i];
	line.clearScore();

	currentLines.remove(i);
	return true;
}

bool Training::createLines(QWidget* parent, int color, ChessBoard& scb)
{
	QVector<BookDBEntry> pos;
	ChessBoard cb;
	TrainingLine line;

	int i, j, rep, score;

	int steps = 19, step;
	
	QProgressDialog progress("Creating trainingdata...", "Abort", 0, steps, parent);
	progress.setWindowModality(Qt::WindowModal);
	progress.show();
	QApplication::processEvents();
	step = 0;

	currentLines.clear();
	for (rep = 0; rep < 2; rep++)
	{
		progress.setLabelText("Reading positions from database ...");
		progress.setValue(++step);
		QApplication::processEvents();
		if (progress.wasCanceled())
		{
			progress.setValue(steps);
			return false;
		}
		if (color != -1)
		{
			if (color != rep)
			{
				step += 2;
				continue;
			}
		}

		// Is the base in use
		if (!Base[rep])
		{
			step += 2;
			continue;
		}
		// Collect all lines
		Base[rep]->getTrainingPosition(pos);

		QApplication::processEvents();
		if (progress.wasCanceled())
		{
			progress.setValue(steps);
			return false;
		}
		progress.setLabelText("Sorting positions...");
		progress.setValue(++step);
		if (pos.size() > 0)
		{
			QApplication::processEvents();
			std::sort(pos.begin(), pos.end());
			progress.setLabelText("Creating lines ...");
			progress.setValue(++step);
			QApplication::processEvents();
			if (progress.wasCanceled())
			{
				progress.setValue(steps);
				return false;
			}
			cb.setStartposition();
			walkThrough(cb, line, 0, pos, rep);
		}
		else
		{
			++step;
		}
	}
	// Step=6
	progress.setLabelText("Tuning lines ...");
	progress.setValue(++step);
	QApplication::processEvents();
	if (progress.wasCanceled())
	{
		progress.setValue(steps);
		return false;
	}

	// Remove last move if it not a repertoire move
	for (i = 0; i < currentLines.size(); i++)
	{
		j = currentLines[i].moves.size() % 2;
		if ((currentLines[i].color == WHITE) && (j == 0))
			currentLines[i].moves.pop_back();
		else if ((currentLines[i].color == BLACK) && (j == 1))
			currentLines[i].moves.pop_back();
	}

	// Remove lines which don't have the search position
	if (!scb.isStartposition())
	{
		bool found;
		for (i = 0; i < currentLines.size(); i++)
		{
			found = false;
			cb.setStartposition();
			for (j = 0; j < currentLines[i].moves.size(); j++)
			{
				if (cb == scb)
				{
					found = true;
					break;
				}
				cb.doMove(currentLines[i].moves[j].move, false);
			}
			if (!found)
			{
				currentLines.remove(i);
				--i;
			}
		}
	}

	stat.inBase = currentLines.size();

	if (!currentLines.size())
	{
		progress.setValue(steps);
		return false;
	}

	progress.setLabelText("Update score ...");
	progress.setValue(++step);
	QApplication::processEvents();
	if (progress.wasCanceled())
	{
		progress.setValue(steps);
		return false;
	}

	// Update score
	for (i = 0; i < currentLines.size(); i++)
		currentLines[i].scoreLine();

	progress.setLabelText("Sorting lines ...");
	progress.setValue(++step);
	QApplication::processEvents();
	if (progress.wasCanceled())
	{
		progress.setValue(steps);
		return false;
	}
	
	// Sort list based on score;
	std::sort(currentLines.begin(), currentLines.end());

	progress.setLabelText("Keep only lowest score ...");
	progress.setValue(++step);
	QApplication::processEvents();
	if (progress.wasCanceled())
	{
		progress.setValue(steps);
		return false;
	}

	// Keep only lowest score
	score = currentLines[0].score;
	for (i = 1; i < currentLines.size(); i++)
	{
		if (currentLines[i].score > score)
		{
			currentLines.remove(i, currentLines.size() - i);
			break;
		}
	}

	progress.setValue(steps);
	return true;
}
