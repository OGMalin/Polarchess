#include "Statistics.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFileDialog>
#include <QProgressDialog>
#include <QApplication>
#include <QVariantList>
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

// Doing this in at bit strange way to try to speed up the database handling., insert on a SqlLight db are very slow.
void Statistics::importGames(QWidget* parent)
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
		if (cb.toMove == WHITE)
		{
			if (game.info.WhiteElo.size())
				sdm.elo = stoi(game.info.WhiteElo);
		}
		else
		{
			if (game.info.BlackElo.size())
				sdm.elo = stoi(game.info.BlackElo);
		}
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
		query.prepare("SELECT * FROM positions WHERE hash = :hash;");
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
	QSqlDatabase db = QSqlDatabase::database(STATISTICS);
	if (!opened)
		return;
	if (!db.open())
		return;
	QSqlQuery query(db);

	QVariantList cboardlist;
	QByteArray cboard;

	// Open progress dialog
	QProgressDialog progress("Searching for single games in statistics.", "Cancel", 0, 100, parent);
	progress.setWindowModality(Qt::WindowModal);
	progress.setMinimumDuration(0);
	progress.show();

	QApplication::processEvents();
	query.exec("SELECT * FROM positions;");
	while (query.next())
	{
		if (haveSingleMove(query.value("movelist").toString()))
		{
			cboardlist.push_back(query.value("cboard"));
		}
	}
	progress.setLabelText("Removing single games in statistics.");

	QApplication::processEvents();
	if (progress.wasCanceled())
	{
		return;
	}
	int next = 0;
	int i;
	progress.setMaximum(cboardlist.size());
	while (next < cboardlist.size())
	{
		db.transaction();
		for (i = 0; i < 100; i++)
		{
			if (next>= cboardlist.size())
				break;
			query.prepare("DELETE FROM positions WHERE hash=:hash;");
			query.bindValue(":cboard", cboardlist[next]);
			query.exec();
			++next;
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
		QSqlError error = query.lastError();
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

