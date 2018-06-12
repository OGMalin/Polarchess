#include "Database.h"
#include <QCoreApplication>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QDir>
#include <QVariant>
#include <QDebug>

const char* GAMEDBVERSION = "1.0";
const char* GAMEDBTYPE = "GAMEDB";
const char* BOOKDBVERSION = "1.0";
const char* BOOKDBTYPE = "BOOKDB";


Database::Database(QObject *parent)
	: QObject(parent)
{
	if (!QSqlDatabase::isDriverAvailable("QSQLITE"))
	{
		qWarning("No SQLITE driver available.");
		return;
	}
	QString path = QStandardPaths::locate(QStandardPaths::DocumentsLocation,"/Polarchess", QStandardPaths::LocateDirectory);
	if (path.isEmpty())
	{
		QStringList l = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
		if (l.size())
		{
			path = l.front();
			path += "/Polarchess";
			QDir().mkdir(path);
		}

	}
		
	gamedb = QSqlDatabase::addDatabase("QSQLITE");
	gamedb.setDatabaseName(path+ "/Polarchess.games");
	path = QStandardPaths::locate(QStandardPaths::DocumentsLocation, "/Polarchess/Polarchess.games", QStandardPaths::LocateFile);
	if (path.isEmpty())
		create();
	bookdb.setDatabaseName(path + "/Polarchess.book");
}

Database::~Database()
{
}

bool Database::create()
{
	if (!gamedb.open())
		return false;

	QSqlQuery query(gamedb);
	query.exec("CREATE TABLE info ( id INTEGER, type TEXT, version TEXT);");
	query.prepare("INSERT INTO info (type, version) VALUES ( :type, :version);");
	query.bindValue(":type", GAMEDBTYPE);
	query.bindValue(":version", GAMEDBVERSION);
	query.exec();

	query.exec("CREATE TABLE `games` ( "
		"`id`	INTEGER,"
		"`event`	TEXT,"
		"`site`	TEXT,"
		"`white`	TEXT,"
		"`black`	TEXT,"
		"`round`	TEXT,"
		"`date`	TEXT,"
		"`result`	TEXT,"
		"`whiteelo`	TEXT,"
		"`blackelo`	TEXT,"
		"`eco`	TEXT,"
		"`annotator`	TEXT,"
		"`whitetimecontrol`	TEXT,"
		"`blacktimecontrol`	TEXT,"
		"`rated`	TEXT,"
		"`analysisengine`	TEXT,"
		"`movetext`	BLOB,"
		"PRIMARY KEY(`id`,`white`,`black`,`date`)"
		"); ");
	gamedb.close();
	return true;
}

bool Database::addGame(QChessGame* game)
{
/*
char sz[1024];
	QSqlQuery query;
	if (!db.open())
		return false;
	sprintf_s(sz, 1024, "INSERT INTO games (event, site, white, black,round, date, result, whiteelo, blackelo, "
		"eco, annotator, whitetype, blacktype, whitetimecontrol, blacktimecontrol, opening, "
		"variation, subvariation, remark, movetext ) VALUES ("
		"'%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')",
		game.info.Event.c_str(),
		game.info.Site.c_str(),
		game.info.White.c_str(),
		game.info.Black.c_str(),
		game.info.Round.c_str(),
		game.info.Date.c_str(),
		game.info.Result.c_str(),
		game.info.WhiteElo.c_str(),
		game.info.BlackElo.c_str(),
		game.info.WhiteType.c_str(),
		game.info.BlackType.c_str(),
		game.info.WhiteTimeControl.c_str(),
		game.info.BlackTimeControl.c_str(),
		game.info.ECO.c_str(),
		game.info.Opening.c_str(),
		game.info.Variation.c_str(),
		game.info.SubVariation.c_str(),
		game.info.Remark.c_str(),
		game.info.Annotator.c_str(),
		game.toString().c_str());
		*/
	return true;
}