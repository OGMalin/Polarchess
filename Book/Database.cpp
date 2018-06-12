#include "Database.h"
#include <QCoreApplication>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QDir>
#include <QVariant>
#include <QSqlError>
#include <QDebug>

const char* DBVERSION = "1.0";
const char* DBTYPE = "BOOKDB";

Database::Database(QObject *parent)
	: QObject(parent)
{
	opened = false;
	if (!QSqlDatabase::isDriverAvailable("QSQLITE"))
	{
		qWarning("No SQLITE driver available.");
		return;
	}
	bookdb = QSqlDatabase::addDatabase("QSQLITE");
}

Database::~Database()
{
}

bool Database::open(const QString& path)
{
	bookdb.setDatabaseName(path);

	if (!bookdb.open())
	{
		return false;
	}
	bookdb.close();
	opened = true;
	return true;
}

bool Database::create(const QString& path)
{
	bookdb.setDatabaseName(path);
	
	if (!bookdb.open())
	{
		qDebug() << "Create database error. Database: " << bookdb.lastError().databaseText() << "Driver: " << bookdb.lastError().driverText();
		return false;
	}
	QSqlQuery query(bookdb);
	query.exec("CREATE TABLE info ( id INTEGER, type TEXT, version TEXT, PRIMARY KEY(id));");
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
	opened = true;
	return true;
}

bool Database::add(BookDBEntry& data)
{
	if (!opened)
		return false;
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