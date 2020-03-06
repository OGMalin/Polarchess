#include "Statistics.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFileDialog>
#include <QProgressDialog>
#include <QApplication>
#include <QVariantList>
#include <QtAlgorithms>
#include "../Common/Pgn.h"

using namespace std;
const char* STATISTICS = "Statistics";
const char* SDBVERSION = "1.0";
const char* SDBTYPE = "POLARSTATISTICSDB";

Statistics::Statistics(QObject *parent)
	: QObject(parent)
{
	opened = false;
	if (!QSqlDatabase::isDriverAvailable("QSQLITE"))
	{
		qWarning("No SQLITE driver available.");
		return;
	}
	QSqlDatabase::addDatabase("QSQLITE", STATISTICS);
}

Statistics::~Statistics()
{
}

bool Statistics::open(const QString& path)
{
	if (!WinFile::exist(path.toStdString()))
		return false;
	QSqlDatabase db = QSqlDatabase::database(STATISTICS);
	db.setDatabaseName(path);

	if (!db.open())
	{
		opened = false;
	}
	else
	{
		opened = true;
		QSqlQuery query(db);
		query.prepare("SELECT * FROM info;");
		if (query.exec() && query.next())
		{
			sdi.db = query.value("db").toString();
			sdi.version = query.value("version").toString();
		}
		QSqlError error = query.lastError();
		if (error.isValid())
		{
			qDebug() << "Database error: " << error.databaseText();
			qDebug() << "Driver error: " << error.driverText();
		}
	}

	return opened;
}

bool Statistics::create(const QString& path)
{
	QSqlDatabase db = QSqlDatabase::database(STATISTICS);
	char sz[16];

	db.setDatabaseName(path);

	if (!db.open())
	{
		qDebug() << "Create database error. Database: " << db.lastError().databaseText() << "Driver: " << db.lastError().driverText();
		opened = false;
		return false;
	}
	QSqlQuery query(db);
	query.exec("CREATE TABLE info ( db TEXT, version TEXT);");
	query.prepare("INSERT INTO info (db, version) VALUES ( :db, :version);");
	query.bindValue(":db", SDBTYPE);
	query.bindValue(":version", SDBVERSION);
	query.exec();
	query.exec("CREATE TABLE positions ( "
		"cboard	BLOB, "
		"movelist	TEXT, "
		"PRIMARY KEY (cboard)"
		" ); ");
	QSqlError error = query.lastError();
	if (error.isValid())
	{
		qDebug() << "Database error: " << error.databaseText();
		qDebug() << "Driver error: " << error.driverText();
	}
	sdi.db = SDBTYPE;
	sdi.version = SDBVERSION;
	opened = true;
	return true;
}

void Statistics::close()
{
	opened = false;
}

StatisticsDBEntry Statistics::find(ChessBoard& cb)
{
	StatisticsDBEntry sde;

	if (!opened)
		return sde;

	QSqlDatabase db = QSqlDatabase::database(STATISTICS);
	if (!db.open())
		return sde;

	QSqlQuery query(db);
	QByteArray cboard = CompressedBoard::compress(cb);
	query.prepare("SELECT * FROM positions WHERE cboard = :cboard;");
	query.bindValue(":cboard", cboard);
	if (query.exec() && query.next())
	{
		sde.convertToMoveList(sde.movelist, query.value("movelist").toString(), cb);
		sde.cboard = cboard;
	}
	QSqlError error = query.lastError();
	if (error.isValid())
	{
		qDebug() << "Database error: " << error.databaseText();
		qDebug() << "Driver error: " << error.driverText();
	}
	return sde;
}

//void Statistics::addMove(StatisticsDBMove& m, ChessBoard& cb)
//{
//	QSqlDatabase db = QSqlDatabase::database(STATISTICS);
//	StatisticsDBEntry sde;
//	bool exist;
//
//	char sz[16];
//	if (!opened)
//		return;
//	if (!db.open())
//		return;
//
//	QSqlQuery query(db);
//
//	exist = find(sde, cb);
//
//	sde.updateMove(m);
//
//	if (exist)
//	{
//		query.prepare("UPDATE positions SET "
//			"movelist = :movelist "
//			"WHERE hash = :hash;");
//	}
//	else
//	{
//		query.prepare("INSERT INTO positions ( "
//			"hash, movelist"
//			") VALUES ( "
//			":hash, :movelist );");
//	}
//	query.bindValue(":hash", sde.hash);
//	QString qs;
//	sde.convertFromMoveList(sde.movelist, qs, cb);
//	query.bindValue(":movelist", qs);
//	query.exec();
//	QSqlError error = query.lastError();
//	if (error.isValid())
//	{
//		qDebug() << "Database error: " << error.databaseText();
//		qDebug() << "Driver error: " << error.driverText();
//	}
//	return;
//}

void Statistics::importGames(QWidget* parent)
{
#ifdef _DEBUG
	QSqlError error;
#endif
	Pgn pgn;
	int gameindex, i, j;
	QVector<StatisticsDBEntry> list;
	QVector<StatisticsDBEntry> dblist;
	StatisticsDBEntry sde;
	QString qs;
	QSqlDatabase db = QSqlDatabase::database(STATISTICS);
	if (!opened)
		return;
	if (!db.open())
		return;
	QSqlQuery query(db);
	// Get pgn filename
	QString path = QFileDialog::getOpenFileName(parent, "Open pgnfile", QString(), "Pgn files (*.pgn)");
	if (path.isEmpty())
		return;
	if (!pgn.open(path.toStdString(), true))
		return;
	// Open progress dialog
	QProgressDialog progress("Importing Pgn file.", "Cancel", 0, pgn.file.size(), parent);
	progress.setWindowModality(Qt::WindowModal);
	progress.setMinimumDuration(0);
	progress.show();
	progress.setLabelText("Reading games");
	QApplication::processEvents();

	gameindex = 1;
	while (readGames(list, pgn, gameindex))
	{
		progress.setLabelText("Reading statistics");
		QApplication::processEvents();
		if (progress.wasCanceled())
		{
			pgn.close();
			return;
		}

		// Check the database if a position allready exist
		dblist.clear();
		query.prepare("SELECT rowid, movelist FROM positions WHERE cboard = :cboard;");
		for (i = 0; i < list.size(); i++)
		{
			query.bindValue(":cboard", list[i].cboard);
			if (query.exec() && query.next())
			{
				sde.convertToMoveList(sde.movelist, query.value("movelist").toString(), list[i].cboard);
				sde.cboard = list[i].cboard;
				sde.rowid = query.value("rowid").toUInt();
				for (j = 0; j < list[i].movelist.size(); j++)
					sde.updateMove(list[i].movelist[j]);
				dblist.push_back(sde);
				list[i].rowid = -1; // Mark as found and updated
			}
#ifdef _DEBUG
			else
			{
				error = query.lastError();
				if (error.isValid())
				{
					qDebug() << "Database error: " << error.databaseText();
					qDebug() << "Driver error: " << error.driverText();
				}
			}
#endif
		}
		progress.setLabelText("Saving to statistics");
		QApplication::processEvents();
		if (progress.wasCanceled())
		{
			pgn.close();
			return;
		}

		db.transaction();
		query.prepare("UPDATE positions SET "
			"movelist = :movelist "
			"WHERE rowid = :rowid;");
		for (i = 0; i < dblist.size(); i++)
		{
			dblist[i].convertFromMoveList(dblist[i].movelist, qs, dblist[i].cboard);
			query.bindValue(":movelist", qs);
			query.bindValue(":rowid", dblist[i].rowid);
			query.exec();
#ifdef _DEBUG
			error = query.lastError();
			if (error.isValid())
			{
				qDebug() << "Database error: " << error.databaseText();
				qDebug() << "Driver error: " << error.driverText();
			}
#endif
		}
//		db.commit();

//		db.transaction();
		query.prepare("INSERT INTO positions ( "
			"cboard, movelist"
			") VALUES ( "
			":cboard, :movelist );");
		for (i = 0; i < list.size(); i++)
		{
			if (list[i].rowid != -1)
			{
				list[i].convertFromMoveList(list[i].movelist, qs, list[i].cboard);
				query.bindValue(":movelist", qs);
				query.bindValue(":cboard", list[i].cboard);
				query.exec();
#ifdef _DEBUG
				error = query.lastError();
				if (error.isValid())
				{
					qDebug() << "Database error: " << error.databaseText();
					qDebug() << "Driver error: " << error.driverText();
				}
#endif
			}
		}
		db.commit();

		progress.setLabelText("Reading games");
		progress.setValue(pgn.file.dwFilepointer);
		QApplication::processEvents();
		if (progress.wasCanceled())
		{
			pgn.close();
			return;
		}
	}
	progress.setValue(pgn.file.size());
	pgn.close();
}

// Read the 20 first moves from 1000 games and put each position into list
bool Statistics::readGames(QVector<StatisticsDBEntry>& list, Pgn& pgn, int& index)
{
	SYSTEMTIME st;
	int i,wElo,bElo;
	ChessGame game;
	ChessBoard cb;
	StatisticsDBMove sdm;
	StatisticsDBEntry sde;
	QVector<StatisticsDBEntry>::iterator lit;
	std::string ss;

	list.clear();

	for (i = 0; i < 1000; i++)
	{
		if (!pgn.read(game, index++, 20))
			break;
		sdm.clear();
		if (game.info.Result == "1-0")
			sdm.whitewin = 1;
		else if (game.info.Result == "0-1")
			sdm.blackwin = 1;
		else if (game.info.Result.substr(0, 3) == "1/2")
			sdm.draw = 1;
		else
			continue;
		wElo = bElo = 0;
		if (game.info.WhiteElo.size())
			wElo = stoi(game.info.WhiteElo);
		if (game.info.BlackElo.size())
			bElo = stoi(game.info.BlackElo);
		game.getDate(st);
		sdm.year = st.wYear;
		game.getStartPosition(cb);
		game.toStart();
		while (game.getMove(sdm.move, ss, 0))
		{
			if (cb.toMove == WHITE)
				sdm.elo = wElo;
			else
				sdm.elo = bElo;

			sde.clear();
			sde.cboard = CompressedBoard::compress(cb);
			lit = qBinaryFind(list.begin(), list.end(), sde);
			if (lit == list.end())
			{
				sde.movelist.push_back(sdm);
				lit = qLowerBound(list.begin(), list.end(), sde);
				list.insert(lit, sde);
			}
			else
			{
				lit->updateMove(sdm);
			}
			game.doMove(0);
			game.getPosition(cb);
		}
	}
	if (!list.size())
		return false;
	return true;
}
// Doing this in at bit strange way to try to speed up the database handling., insert on a SqlLight db are very slow.
void Statistics::importGames2(QWidget* parent)
{
	SYSTEMTIME st;
	StatisticsDBMove sdm;
	StatisticsDBEntry sde;
	ChessBoard cb;
	QVector<StatisticsDBEntry> sdes;
	std::string ss;
	Pgn pgn;
	ChessGame game;
	QByteArray cboard;
	int i;
	QSqlDatabase db = QSqlDatabase::database(STATISTICS);
	if (!opened)
		return;
	if (!db.open())
		return;

	// Get pgn filename
	QString path = QFileDialog::getOpenFileName(parent, "Open pgnfile", QString(), "Pgn files (*.pgn)");
	if (path.isEmpty())
		return;
	if (!pgn.open(path.toStdString(), true))
		return;

	// Open progress dialog
	QProgressDialog progress("Importing Pgn file.", "Cancel", 0, pgn.file.size(), parent);
	progress.setWindowModality(Qt::WindowModal);
	progress.setMinimumDuration(0);
	progress.show();

	QSqlQuery query(db);
	QString qs;
	int wElo = 0, bElo = 0;
	int next = 1;
	while (pgn.read(game, next++, 20))
	{
		sdm.clear();
		if (game.info.Result == "1-0")
			sdm.whitewin = 1;
		else if (game.info.Result == "0-1")
			sdm.blackwin = 1;
		else if (game.info.Result.substr(0,3) == "1/2")
			sdm.draw = 1;
		else
			continue;
		wElo = bElo = 0;
		if (game.info.WhiteElo.size())
			wElo = stoi(game.info.WhiteElo);
		if (game.info.BlackElo.size())
			bElo = stoi(game.info.BlackElo);
		game.getDate(st);
		sdm.year = st.wYear;
		progress.setValue(pgn.file.dwFilepointer);
		QApplication::processEvents();
		if (progress.wasCanceled())
		{
			pgn.close();
			return;
		}
		game.getStartPosition(cb);
		game.toStart();
		// Collect all positions from db
		sdes.clear();
		query.prepare("SELECT * FROM positions WHERE cboard = :cboard;");
		while (game.getMove(sdm.move, ss, 0))
		{
			sde.clear();
			sde.cboard = CompressedBoard::compress(cb);
			query.bindValue(":cboard", sde.cboard);
			if (query.exec() && query.next())
			{
				sde.convertToMoveList(sde.movelist, query.value("movelist").toString(), cb);
				sdes.push_back(sde);
			}
			game.doMove(0);
			game.getPosition(cb);
		}

		//
		game.getStartPosition(cb);
		game.toStart();
		db.transaction();
		while (game.getMove(sdm.move, ss, 0))
		{
			if (cb.toMove == WHITE)
				sdm.elo = wElo;
			else
				sdm.elo = bElo;

			sde.cboard = CompressedBoard::compress(cb);
			for (i = 0; i < sdes.size(); i++)
				if (sdes[i].cboard == sde.cboard)
					break;
			// New record
			if (i >= sdes.size())
			{
				sde.movelist.clear();
				sde.movelist.push_back(sdm);
				query.prepare("INSERT INTO positions ( "
					"cboard, movelist"
					") VALUES ( "
					":cboard, :movelist );");
				sde.convertFromMoveList(sde.movelist, qs, cb);
				query.bindValue(":movelist", qs);
				query.bindValue(":cboard", sde.cboard);
			}
			// Update old record
			else
			{
				sdes[i].updateMove(sdm);
				query.prepare("UPDATE positions SET "
					"movelist = :movelist "
					"WHERE cboard = :cboard;");
				sdes[i].convertFromMoveList(sdes[i].movelist, qs, cb);
				query.bindValue(":movelist", qs);
				query.bindValue(":cboard", sde.cboard);
			}
			query.exec();

			game.doMove(0);
			game.getPosition(cb);
		}
		db.commit();
	}
	progress.setValue(pgn.file.size());
}

void Statistics::removeSingleGame(QWidget* parent)
{
	QSqlError error;
	QSqlDatabase db = QSqlDatabase::database(STATISTICS);
	if (!opened)
		return;
	if (!db.open())
		return;
	QSqlQuery query(db);

	QVector<unsigned int> rowidlist;

	// Open progress dialog
	QProgressDialog progress("Searching for single games in statistics.", "Cancel", 0, 100, parent);
	progress.setWindowModality(Qt::WindowModal);
	progress.setMinimumDuration(0);
	progress.show();

	QApplication::processEvents();
	query.exec("SELECT rowid, movelist FROM positions;");
	while (query.next())
		if (haveSingleMove(query.value("movelist").toString()))
			rowidlist.push_back(query.value("rowid").toUInt());

	progress.setLabelText("Removing single games in statistics.");

	QApplication::processEvents();
	if (progress.wasCanceled())
	{
		return;
	}
	int next = 0;
	int i;
	progress.setMaximum(rowidlist.size());
	while (next < rowidlist.size())
	{
		db.transaction();
		for (i = 0; i < 1000; i++)
		{
			if (next>= rowidlist.size())
				break;
			query.prepare("DELETE FROM positions WHERE rowid=:rowid;");
			query.bindValue(":rowid", rowidlist[next]);
			query.exec();
			++next;
#ifdef _DEBUG
			error = query.lastError();
			if (error.isValid())
			{
				qDebug() << "Database error: " << error.databaseText();
				qDebug() << "Driver error: " << error.driverText();
			}
#endif
		}
		db.commit();
		progress.setValue(next);
		QApplication::processEvents();
		if (progress.wasCanceled())
		{
			return;
		}
	}
	progress.setLabelText("Compacting statistics.");
	QApplication::processEvents();
	if (progress.wasCanceled())
	{
		return;
	}
	if (!query.exec("VACUUM;"))
	{
		error = query.lastError();
		qDebug() << "Database error: " << error.databaseText();
		qDebug() << "Driver error: " << error.driverText();
	}
}

bool Statistics::haveSingleMove(QString& qs)
{
	if (qs.contains(';'))
		return false;
	QStringList sl= qs.split('|');
	int games = 0;
	if (sl.size() > 1)
		games += sl[1].toInt();
	if (sl.size() > 2)
		games += sl[2].toInt();
	if (sl.size() > 3)
		games += sl[3].toInt();
	if (games < 2)
		return true;
	return false;
}

QString Statistics::getPath()
{
	QSqlDatabase db = QSqlDatabase::database(STATISTICS);
	return db.databaseName();
}

