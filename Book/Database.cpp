#include "Database.h"
#include <QCoreApplication>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QDir>
#include <QVariant>

//#include <QDebug>

const char* DBVERSION = "1.0";
const char* DBTYPE = "BOOKDB";

Database::Database(QObject *parent)
	: QObject(parent)
{
	if (!QSqlDatabase::isDriverAvailable("QSQLITE"))
	{
		qWarning("No SQLITE driver available.");
		return;
	}
	/*
	QString path = QStandardPaths::locate(QStandardPaths::DocumentsLocation, "/Polarchess", QStandardPaths::LocateDirectory);
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

	path += "/Book.db";
	db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName(path);
	path = QStandardPaths::locate(QStandardPaths::DocumentsLocation, "/Polarchess/Book.db", QStandardPaths::LocateFile);
	if (path.isEmpty())
		create();
		*/
}

Database::~Database()
{
}

bool Database::open(const QString& path)
{
	bookdb.setDatabaseName(path);

	if (!bookdb.open())
		return false;
	bookdb.close();
	return true;
}

bool Database::create(const QString& path)
{
	bookdb.setDatabaseName(path);
	
	if (!bookdb.open())
		return false;

	QSqlQuery query(bookdb);
	query.exec("CREATE TABLE info ( id INTEGER, type TEXT, version TEXT);");
	query.prepare("INSERT INTO info (type, version) VALUES ( :type, :version);");
	query.bindValue(":type", DBTYPE);
	query.bindValue(":version", DBVERSION);
	query.exec();
	query.exec("CREATE TABLE `positions` ( "
		"id	INTEGER,"
		"fen	TEXT,"
		"opening	TEXT,"
		"variation	TEXT,"
		"subvariation	TEXT,"
		"eco	TEXT,"
		"comment	TEXT,"
		"movelist	BLOB,"
		"PRIMARY KEY(id,fen)"
		"); ");
	bookdb.close();
	return true;
}

bool Database::add(BookDBEntry& data)
{
	if (!bookdb.open())
		return false;
	QSqlQuery query(bookdb);

	if (data.id)
	{

	}
	else
	{

	}
	query.prepare("");
	bookdb.close();
	return true;
}