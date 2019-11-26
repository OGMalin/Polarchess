#include "Statistics.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

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
		"fen	TEXT,"
		"movelist	TEXT,"
		"PRIMARY KEY(fen)"
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

bool Statistics::find(StatisticsDBEntry& sde)
{
	if (!opened)
		return false;
	QSqlDatabase db = QSqlDatabase::database(STATISTICS);
	if (!db.open())
		return false;
	QSqlQuery query(db);
	query.prepare("SELECT * FROM positions WHERE fen = :fen;");
	query.bindValue(":fen", sde.board.getFen(true).c_str());
	if (query.exec() && query.next())
	{
		sde.convertToMoveList(sde.movelist, query.value("movelist").toString());
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
	sde.board = cb;

	exist = find(sde);

	sde.updateMove(m);

	if (exist)
	{
		query.prepare("UPDATE positions SET "
			"movelist = :movelist "
			"WHERE fen = :fen;");
	}
	else
	{
		query.prepare("INSERT INTO positions ( "
			"fen, movelist"
			") VALUES ( "
			":fen, :movelist );");
	}
	query.bindValue(":fen", sde.board.getFen(true).c_str());
	QString qs;
	sde.convertFromMoveList(sde.movelist, qs);
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


void StatisticsDBEntry::convertToMoveList(QVector<StatisticsDBMove>& movelist, const QString& data)
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
			sdm.move = board.getMoveFromText(qmove[0].toStdString());
		if (qmove.size() > 1)
			sdm.win = qmove[1].toInt();
		if (qmove.size() > 2)
			sdm.draw = qmove[2].toInt();
		if (qmove.size() > 3)
			sdm.loss = qmove[3].toInt();
		if (qmove.size() > 4)
			sdm.elo = qmove[4].toInt();
		if (qmove.size() > 5)
			sdm.year = qmove[5].toInt();
		if (!sdm.move.empty())
			movelist.append(sdm);
	}
}

void StatisticsDBEntry::convertFromMoveList(const QVector<StatisticsDBMove>& movelist, QString& data)
{
	data.clear();
	char sz[16];
	bool first = true;
	QVector<StatisticsDBMove>::ConstIterator it = movelist.begin();
	while (it != movelist.end())
	{
		if (board.isLegal(ChessMove(it->move)))
		{
			if (!first)
				data += ";";
			else
				first = false;
			data += board.makeMoveText(it->move, sz, 16, FIDE);
			data += "|";
			data += itoa(it->win, sz, 10);
			data += "|";
			data += itoa(it->draw, sz, 10);
			data += "|";
			data += itoa(it->loss, sz, 10);
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
			movelist[i].win += bm.win;
			movelist[i].draw += bm.draw;
			movelist[i].loss += bm.loss;
			movelist[i].elo = __max(movelist[i].elo,bm.elo);
			movelist[i].year = __max(movelist[i].year, bm.year);
			return;
		}
	}
	movelist.append(bm);
}

