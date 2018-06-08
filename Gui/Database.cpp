#include "Database.h"
#include <QCoreApplication>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

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
		
	path += "/MyGames.db";
	db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName(path);
	path = QStandardPaths::locate(QStandardPaths::DocumentsLocation, "/Polarchess/MyGames.db", QStandardPaths::LocateFile);
	if (path.isEmpty())
		create();
}

Database::~Database()
{
}

bool Database::create()
{
	if (!db.open())
		return false;

	QSqlQuery query;
	query.exec("CREATE TABLE `info` ( `id`	INTEGER, `version` TEXT, PRIMARY KEY(`id`));");
	query.exec("INSERT INTO `info` (`version`) VALUES ( `1.0`));");
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
	db.close();
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