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

void Training::updateScore(TrainingLine& tl)
{
	int i;
	ChessBoard cb;

	// Don't save score/attempt in position before start of training position.
	cb.setStartposition();
	for (i = 0; i < tl.moves.size(); i++)
	{
		if (cb == startBoard)
			break;
		tl.moves[i].score = 0;
		tl.moves[i].attempt = 0;
		cb.doMove(tl.moves[i].move, false);
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

bool Training::createLines(QWidget* parent, int color, ChessBoard& scb, int moves)
{
	QVector<BookDBEntry> pos;
	ChessBoard cb;
	TrainingLine line;

	int i, j, rep, score;

	int steps = 11, step;
	
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

	// Remove moves after last allowed move
	if (moves)
	{
		for (i = 0; i < currentLines.size(); i++)
		{
			if ((moves * 2) < currentLines[i].moves.size())
			{
				j = currentLines[i].moves.size() / 2;
				if (moves < j)
					currentLines[i].moves.remove(moves * 2, currentLines[i].moves.size() - (moves * 2));
			}
		}
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

	// Remove dublicated lines
	for (i = 0; i < currentLines.size()-1; i++)
		for (j = i + 1; j < currentLines.size(); j++)
			if (equalLine(currentLines[i], currentLines[j]))
				currentLines.remove(j--);

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
	{
		currentLines[i].scoreLine(scb);
		if (currentLines[i].score < 0)
		{
			currentLines.remove(i);
			--i;
		}
	}

	stat.inBase = currentLines.size();

	if (!currentLines.size())
	{
		progress.setValue(steps);
		return false;
	}

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

bool Training::equalLine(TrainingLine& tl1, TrainingLine& tl2)
{
	ChessBoard cb;
	int s1, s2, i;

	cb.setStartposition();
	for (s1 = 0; s1 < tl1.moves.size(); s1++)
	{
		if (cb == startBoard)
			break;
		cb.doMove(tl1.moves[s1].move, false);
	}
	if (s1 == tl1.moves.size())
		return false;
	cb.setStartposition();
	for (s2 = 0; s2 < tl2.moves.size(); s2++)
	{
		if (cb == startBoard)
			break;
		cb.doMove(tl2.moves[s2].move, false);
	}
	if (s2 == tl2.moves.size())
		return false;
	if ((tl1.moves.size() - s1) != (tl2.moves.size() - s2))
		return false;
	while (1)
	{
		if (tl1.moves[s1].move != tl2.moves[s2].move)
			return false;
		++s1;
		++s2;
		if (s1 == tl1.moves.size())
			return true;
	}
	return true;
}