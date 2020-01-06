#include "Statistics.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFileDialog>
#include <QProgressDialog>
#include <QApplication>
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
		"hash	TEXT,"
		"movelist	TEXT,"
		"PRIMARY KEY(hash)"
		"); ");
	sdi.db = SDBTYPE;
	sdi.version = SDBVERSION;
	opened = true;
	return true;
}

void Statistics::close()
{
	opened = false;
}

bool Statistics::find(StatisticsDBEntry& sde, ChessBoard& cb)
{
	if (!opened)
		return false;
	QSqlDatabase db = QSqlDatabase::database(STATISTICS);
	if (!db.open())
		return false;
	QSqlQuery query(db);
	sde.hash = cb.hashkey();
	query.prepare("SELECT * FROM positions WHERE hash = :hash;");
	query.bindValue(":hash", sde.hash);
	if (query.exec() && query.next())
	{
		sde.convertToMoveList(sde.movelist, query.value("movelist").toString(), cb);
		return true;
	}
	return false;
}

void Statistics::addMove(StatisticsDBMove& m, ChessBoard& cb)
{
	QSqlDatabase db = QSqlDatabase::database(STATISTICS);
	StatisticsDBEntry sde;
	bool exist;

	char sz[16];
	if (!opened)
		return;
	if (!db.open())
		return;

	QSqlQuery query(db);

	exist = find(sde, cb);

	sde.updateMove(m);

	if (exist)
	{
		query.prepare("UPDATE positions SET "
			"movelist = :movelist "
			"WHERE hash = :hash;");
	}
	else
	{
		query.prepare("INSERT INTO positions ( "
			"hash, movelist"
			") VALUES ( "
			":hash, :movelist );");
	}
	query.bindValue(":hash", sde.hash);
	QString qs;
	sde.convertFromMoveList(sde.movelist, qs, cb);
	query.bindValue(":movelist", qs);
	query.exec();
	QSqlError error = query.lastError();
	if (error.isValid())
	{
		qDebug() << "Database error: " << error.databaseText();
		qDebug() << "Driver error: " << error.driverText();
	}
	return;
}

void Statistics::importGames(QWidget* parent)
{
	SYSTEMTIME st;
	StatisticsDBMove sdm;
	StatisticsDBEntry sde;
	ChessBoard cb;
	std::string ss;
	Pgn pgn;
	bool exist;
	ChessGame game;
	QSqlDatabase db = QSqlDatabase::database(STATISTICS);
	if (!opened)
		return;
	if (!db.open())
		return;
	QString path = QFileDialog::getOpenFileName(parent, "Open pgnfile", QString(), "Pgn files (*.pgn)");
	if (path.isEmpty())
		return;
	if (!pgn.open(path.toStdString(), true))
		return;
	QProgressDialog progress("Importing Pgn file.", "Cancel", 0, pgn.file.size(), parent);
	progress.setWindowModality(Qt::WindowModal);
	progress.setMinimumDuration(0);
	progress.show();

	QSqlQuery query(db);
	QString qs;

	int next = 1;
	while (pgn.read(game, next++, 25))
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
		while (game.getMove(sdm.move, ss, 0))
		{
			exist = find(sde, cb);

			sde.updateMove(sdm);

			if (exist)
			{
				query.prepare("UPDATE positions SET "
					"movelist = :movelist "
					"WHERE hash = :hash;");
			}
			else
			{
				query.prepare("INSERT INTO positions ( "
					"hash, movelist"
					") VALUES ( "
					":hash, :movelist );");
			}
			query.bindValue(":hash", sde.hash);
			sde.convertFromMoveList(sde.movelist, qs, cb);
			query.bindValue(":movelist", qs);
			query.exec();
//			addMove(sdm, cb);
			game.doMove(0);
			game.getPosition(cb);
		}
	}
	progress.setValue(pgn.file.size());
}

bool StatisticsDBEntry::moveExist(ChessMove& move)
{
	QVector<StatisticsDBMove>::iterator it = movelist.begin();
	while (it != movelist.end())
	{
		if (it->move == move)
			return true;
		++it;
	}
	return false;
}


void StatisticsDBEntry::convertToMoveList(QVector<StatisticsDBMove>& movelist, const QString& data, ChessBoard& cb)
{
	StatisticsDBMove sdm;
	QStringList qmove;
	QStringList qlist = data.split(';');

	movelist.clear();
	for (int i = 0; i < qlist.size(); i++)
	{
		qmove = qlist[i].split('|');
		sdm.clear();
		if (qmove.size() > 0)
			sdm.move = cb.getMoveFromText(qmove[0].toStdString());
		if (qmove.size() > 1)
			sdm.whitewin = qmove[1].toInt();
		if (qmove.size() > 2)
			sdm.draw = qmove[2].toInt();
		if (qmove.size() > 3)
			sdm.blackwin = qmove[3].toInt();
		if (qmove.size() > 4)
			sdm.elo = qmove[4].toInt();
		if (qmove.size() > 5)
			sdm.year = qmove[5].toInt();
		if (!sdm.move.empty())
			movelist.append(sdm);
	}
}

void StatisticsDBEntry::convertFromMoveList(const QVector<StatisticsDBMove>& movelist, QString& data, ChessBoard& cb)
{
	data.clear();
	char sz[16];
	bool first = true;
	QVector<StatisticsDBMove>::ConstIterator it = movelist.begin();
	while (it != movelist.end())
	{
		if (cb.isLegal(ChessMove(it->move)))
		{
			if (!first)
				data += ";";
			else
				first = false;
			data += cb.makeMoveText(it->move, sz, 16, FIDE);
			data += "|";
			data += itoa(it->whitewin, sz, 10);
			data += "|";
			data += itoa(it->draw, sz, 10);
			data += "|";
			data += itoa(it->blackwin, sz, 10);
			data += "|";
			data += itoa(it->elo, sz, 10);
			data += "|";
			data += itoa(it->year, sz, 10);
		}
		++it;
	}
}

void StatisticsDBEntry::updateMove(StatisticsDBMove& bm)
{
	for (int i = 0; i < movelist.size(); i++)
	{
		if (bm.move == movelist[i].move)
		{
			movelist[i].whitewin += bm.whitewin;
			movelist[i].draw += bm.draw;
			movelist[i].blackwin += bm.blackwin;
			movelist[i].elo = __max(movelist[i].elo,bm.elo);
			movelist[i].year = __max(movelist[i].year, bm.year);
			return;
		}
	}
	movelist.append(bm);
}

