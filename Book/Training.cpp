#include "Training.h"
#include <algorithm>
#include <QProgressDialog>
#include <QApplication>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include "../Common/WinFile.h"

//const char* TRAINING = "Training";
//const char* TDBVERSION = "1.0";
//const char* TDBTYPE = "POLARTRAININGDB";

Training::Training()
{
	stat.inBase = 0;
	stat.current = 0;
	startColor = -1;
	startBoard.setStartposition();
	//currentBoard.clear();
	//currentColor = 0;
	Base[0] = Base[1] = NULL;
	//opened = false;
	//if (!QSqlDatabase::isDriverAvailable("QSQLITE"))
	//{
	//	qWarning("No SQLITE driver available.");
	//	return;
	//}
	//QSqlDatabase::addDatabase("QSQLITE", TRAINING);
}

Training::~Training()
{
//	close();
}

//bool Training::open(const QString& path)
//{
//	if (!WinFile::exist(path.toStdString()))
//		return false;
//
//	QSqlDatabase db = QSqlDatabase::database(TRAINING);
//	db.setDatabaseName(path);
//
//	if (!db.open())
//	{
//		opened = false;
//	}
//	else
//	{
//		opened = true;
//		QSqlQuery query(db);
//
//		query.exec("SELECT * FROM info;");
//		if (query.next())
//		{
//			tdi.db = query.value("db").toString();
//			tdi.version = query.value("version").toString();
//		}
//
//	}
//	return opened;
//}

//bool Training::create(const QString& path)
//{
//	QSqlDatabase db = QSqlDatabase::database(TRAINING);
//	char sz[16];
//
//	db.setDatabaseName(path);
//
//	if (!db.open())
//	{
//		qDebug() << "Create database error. Database: " << db.lastError().databaseText() << "Driver: " << db.lastError().driverText();
//		opened = false;
//		return false;
//	}
//	QSqlQuery query(db);
//	query.exec("CREATE TABLE info ( db TEXT, version TEXT);");
//	query.prepare("INSERT INTO info (db, version) VALUES ( :db, :version);");
//	query.bindValue(":db", TDBTYPE);
//	query.bindValue(":version", TDBVERSION);
//	query.exec();
//	query.exec("CREATE TABLE training ( "
//		"color TEXT,"
//		"score	TEXT,"
//		"moves	TEXT"
//		" ); ");
//	query.exec("CREATE INDEX score on training (score);");
//	query.exec("CREATE INDEX color on training (color);");
//
//	//query.exec("CREATE TABLE traininngstat ( "
//	//	"); ");
//	tdi.db = TDBTYPE;
//	tdi.version = TDBVERSION;
//	opened = true;
//	return true;
//}
//
//void Training::close()
//{
//	opened = false;
//}

void Training::SetRepertoireDatabase(int color, Database* base)
{
	Base[color] = base;
}

//void Training::createLines(QWidget* parent)
//{
//	QVector<BookDBEntry> pos;
//	ChessBoard cb;
//	TrainingDBEntry path;
//	//TrainingLine tline;
//	//QVector<TrainingLine> tlines;
//	int i, j, rep;
//
//	if (!opened)
//		return;
//	QSqlDatabase db = QSqlDatabase::database(TRAINING);
//	if (!db.open())
//		return;
//
//	int steps=19, step;
//	QProgressDialog progress("Creating trainingdata...", "Abort", 0, steps, parent);
//	progress.setWindowModality(Qt::WindowModal);
//	progress.show();
//	QApplication::processEvents();
//	step = 0;
//	
//	list.clear();
//	for (rep = 0; rep < 2; rep++)
//	{
//		progress.setLabelText("Reading positions from database ...");
//		progress.setValue(++step);
//		QApplication::processEvents();
//		if (progress.wasCanceled())
//			break;
//
//		// Should it be done for this base?
//		if (!Base[rep])
//			continue;
//
//		// Collect all lines
//		Base[rep]->getTrainingPosition(pos);
//
//		QApplication::processEvents();
//		if (progress.wasCanceled())
//			break;
//		progress.setLabelText("Sorting positions...");
//		progress.setValue(++step);
//		if (pos.size() > 0)
//		{
//			QApplication::processEvents();
//			std::sort(pos.begin(), pos.end());
//			progress.setLabelText("Creating lines ...");
//			progress.setValue(++step);
//			QApplication::processEvents();
//			if (progress.wasCanceled())
//				break;
//			cb.setStartposition();
//			walkThrough(cb, path, 0, pos, rep);
//		}
//	}
//
//	progress.setLabelText("Tuning lines ...");
//	progress.setValue(++step);
//	QApplication::processEvents();
//	if (progress.wasCanceled())
//		return;
//	
//	// Remove last move if it not a repertoire move
//	for (i = 0; i < list.size(); i++)
//	{
//		j = list[i].moves.size() % 2;
//		if ((list[i].color == WHITE) && (j == 0))
//			list[i].moves.pop_back();
//		else if ((list[i].color == BLACK) && (j == 1))
//			list[i].moves.pop_back();
//	}
//
//	progress.setLabelText("Update score ...");
//	progress.setValue(++step);
//	QApplication::processEvents();
//	if (progress.wasCanceled())
//		return;
//
//	// Update score
//	for (i = 0; i < list.size(); i++)
//		list[i].scoreLine();
//
//	progress.setLabelText("Sorting lines ...");
//	progress.setValue(++step);
//	QApplication::processEvents();
//	if (progress.wasCanceled())
//		return;
//	// Sort list based on score;
//	std::sort(list.begin(), list.end());
//
//	progress.setLabelText("Preparing for saving to database ...");
//	progress.setValue(++step);
//	QApplication::processEvents();
//	if (progress.wasCanceled())
//		return;
//	// Save the list to db;
//
//	QSqlQuery query(db);
////	QVariantList colors, scores, moves;
//
//	progress.setLabelText("Deleting old lines ...");
//	progress.setValue(++step);
//	QApplication::processEvents();
//	if (progress.wasCanceled())
//		return;
//	query.exec("DELETE FROM training;");
//
//	progress.setLabelText("Saving to database ...");
//	progress.setValue(++step);
//	QApplication::processEvents();
//	if (progress.wasCanceled())
//		return;
//	i = 0;
//	while (i < list.size())
//	{
//		// Save 100 entries to the database each time
//		db.transaction();
//		for (j = 0; j < 100; j++)
//		{
//			if (i>=list.size())
//				break;
//			query.prepare("INSERT INTO training ( color, score, moves ) VALUES ( :color, :score, :moves );");
//			query.bindValue(":color", list[i].color);
//			query.bindValue(":score", list[i].score);
//			query.bindValue(":moves", list[i].MovesToString());
//			query.exec();
//			++i;
//		}
//		db.commit();
//	}
//	stat.inBase = list.size();
//	progress.setValue(steps);
//}
//
//void Training::walkThrough(ChessBoard& cb, TrainingDBEntry& path, int ply, QVector<BookDBEntry>& pos, int color)
//{
//	int  curmove = 0;
//	BookDBEntry bde;
//	QVector<BookDBEntry>::iterator bid;
//	TrainingDBMove tpe;
//
//	// Get position 
//	bde.board = cb;
//	bid = std::lower_bound(pos.begin(), pos.end(), bde);
//
//	// The position don't exist or no moves or repeating move
//	//if ((bid->board != cb) || (bid->movelist.size() == 0) || (bid->dirty))
//	if ((bid->board != cb) || (bid->movelist.size() == 0) || path.positionExist(cb))
//	{
//		path.color = color;
//		list.push_back(path);
//		return;
//	}
//	
//	bid->dirty = true;
//
//	while (1)
//	{
//		tpe.move = bid->movelist[curmove].move;
//		tpe.score = bid->score;
//		tpe.attempt = bid->attempt;
//		path.moves.push_back(tpe);
//		cb.doMove(tpe.move, false);
//		walkThrough(cb, path, ply + 1, pos, color);
//		path.moves.pop_back();
//		// Only first move for repertoire
//		if (bde.board.toMove == color)
//			break;
//		++curmove;
//		if (curmove >= bid->movelist.size())
//			break;
//		cb = bde.board;
//	}
//}

void Training::walkThrough(ChessBoard& cb, TrainingLine& line, int ply, QVector<BookDBEntry>& pos, int color)
{
	//int  curmove = 0;
	//BookDBEntry bde;
	//QVector<BookDBEntry>::iterator bid;
	//TrainingMove tm;

	//// Get position 
	//bde.board = cb;
	//bid = std::lower_bound(pos.begin(), pos.end(), bde);

	//// The position don't exist or no moves or repeating move
	//if ((bid->board != cb) || (bid->movelist.size() == 0) || line.positionExist(cb))
	//{
	//	path.color = color;
	//	list.push_back(path);
	//	return;
	//}

	//bid->dirty = true;

	//while (1)
	//{
	//	tpe.move = bid->movelist[curmove].move;
	//	tpe.score = bid->score;
	//	tpe.attempt = bid->attempt;
	//	path.moves.push_back(tpe);
	//	cb.doMove(tpe.move, false);
	//	walkThrough(cb, path, ply + 1, pos, color);
	//	path.moves.pop_back();
	//	// Only first move for repertoire
	//	if (bde.board.toMove == color)
	//		break;
	//	++curmove;
	//	if (curmove >= bid->movelist.size())
	//		break;
	//	cb = bde.board;
	//}
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
	//char sz[16];
	//int i;
	//tde.scoreLine();
	//QSqlDatabase db = QSqlDatabase::database(TRAINING);
	//if (!db.open())
	//	return;
	//QSqlQuery query(db);
	//query.prepare("SELECT moves FROM training WHERE rowid = :rowid;");
	//query.bindValue(":rowid", itoa(tde.rowid, sz, 10));
	//query.exec();
	//if (!query.next())
	//	return;
	//TrainingDBEntry tdes;
	//tdes.MovesFromString(query.value("moves").toString());
	//if (tde.moves.size() != tdes.moves.size())
	//	return;
	//tdes.color = tde.color;

	//for (i = 0; i < tde.moves.size(); i++)
	//{
	//	tdes.moves[i].score += tde.moves[i].score;
	//	tdes.moves[i].attempt += tde.moves[i].attempt;
	//}
	//tdes.scoreLine();

	//query.prepare("UPDATE training SET score = :score, moves = :moves WHERE rowid = :rowid;");
	//query.bindValue(":rowid", itoa(tde.rowid, sz, 10));
	//query.bindValue(":score", itoa(tdes.score, sz, 10));
	//query.bindValue(":moves", tdes.MovesToString());
	//query.exec();

	//// Update Rep. database
	//Base[tde.color]->updateTrainingScore(tde);
}

//QString Training::getPath()
//{
//	QSqlDatabase db = QSqlDatabase::database(TRAINING);
//	return db.databaseName();
//}

void Training::clearAll()
{
	int i;
	for (i = 0; i < 2; i++)
		Base[i]->clearAllTrainingData();

	//if (!opened)
	//	return;
	//QSqlDatabase db = QSqlDatabase::database(TRAINING);
	//if (!db.open())
	//	return;
	//QSqlQuery query(db);
	//query.exec("DELETE FROM training;");
	//query.exec("VACUUM;");
}

TrainingStatistics Training::getStat()
{
	//QSqlDatabase db = QSqlDatabase::database(TRAINING);
	//if (!db.open())
	//	return stat;
	//QSqlQuery query(db);
	//if (query.exec("SELECT COUNT() FROM training;") && query.next())
	//	stat.inBase = query.value(0).toInt();
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
	return false;
}

bool Training::createLines(QWidget* parent, int color, ChessBoard& cb)
{
	//QVector<BookDBEntry> pos;
	//ChessBoard cb;
	//TrainingLine line;

	////QVector<TrainingLine> tlines;
	//int i, j, rep;

	//int steps = 19, step;
	//QProgressDialog progress("Creating trainingdata...", "Abort", 0, steps, parent);
	//progress.setWindowModality(Qt::WindowModal);
	//progress.show();
	//QApplication::processEvents();
	//step = 0;

	//lines.clear();
	//for (rep = 0; rep < 2; rep++)
	//{
	//	progress.setLabelText("Reading positions from database ...");
	//	progress.setValue(++step);
	//	QApplication::processEvents();
	//	if (progress.wasCanceled())
	//		break;

	//	if (color != -1)
	//		if (color != rep)
	//			continue;

	//	// Is the base in use
	//	if (!Base[rep])
	//		continue;

	//	// Collect all lines
	//	Base[rep]->getTrainingPosition(pos);

	//	QApplication::processEvents();
	//	if (progress.wasCanceled())
	//		break;
	//	progress.setLabelText("Sorting positions...");
	//	progress.setValue(++step);
	//	if (pos.size() > 0)
	//	{
	//		QApplication::processEvents();
	//		std::sort(pos.begin(), pos.end());
	//		progress.setLabelText("Creating lines ...");
	//		progress.setValue(++step);
	//		QApplication::processEvents();
	//		if (progress.wasCanceled())
	//			break;
	//		cb.setStartposition();
	//		walkThrough(cb, line, 0, pos, rep);
	//	}
	//}

	//progress.setLabelText("Tuning lines ...");
	//progress.setValue(++step);
	//QApplication::processEvents();
	//if (progress.wasCanceled())
	//	return;

	//// Remove last move if it not a repertoire move
	//for (i = 0; i < list.size(); i++)
	//{
	//	j = list[i].moves.size() % 2;
	//	if ((list[i].color == WHITE) && (j == 0))
	//		list[i].moves.pop_back();
	//	else if ((list[i].color == BLACK) && (j == 1))
	//		list[i].moves.pop_back();
	//}

	//progress.setLabelText("Update score ...");
	//progress.setValue(++step);
	//QApplication::processEvents();
	//if (progress.wasCanceled())
	//	return;

	//// Update score
	//for (i = 0; i < list.size(); i++)
	//	list[i].scoreLine();

	//progress.setLabelText("Sorting lines ...");
	//progress.setValue(++step);
	//QApplication::processEvents();
	//if (progress.wasCanceled())
	//	return;
	//// Sort list based on score;
	//std::sort(list.begin(), list.end());

	//progress.setLabelText("Preparing for saving to database ...");
	//progress.setValue(++step);
	//QApplication::processEvents();
	//if (progress.wasCanceled())
	//	return;
	//// Save the list to db;

	//QSqlQuery query(db);
	////	QVariantList colors, scores, moves;

	//progress.setLabelText("Deleting old lines ...");
	//progress.setValue(++step);
	//QApplication::processEvents();
	//if (progress.wasCanceled())
	//	return;
	//query.exec("DELETE FROM training;");

	//progress.setLabelText("Saving to database ...");
	//progress.setValue(++step);
	//QApplication::processEvents();
	//if (progress.wasCanceled())
	//	return;
	//i = 0;
	//while (i < list.size())
	//{
	//	// Save 100 entries to the database each time
	//	db.transaction();
	//	for (j = 0; j < 100; j++)
	//	{
	//		if (i >= list.size())
	//			break;
	//		query.prepare("INSERT INTO training ( color, score, moves ) VALUES ( :color, :score, :moves );");
	//		query.bindValue(":color", list[i].color);
	//		query.bindValue(":score", list[i].score);
	//		query.bindValue(":moves", list[i].MovesToString());
	//		query.exec();
	//		++i;
	//	}
	//	db.commit();
	//}
	//stat.inBase = list.size();
	//progress.setValue(steps);
	return false;
}
