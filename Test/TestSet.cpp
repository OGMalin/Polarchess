#include "TestSet.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

const char* DBNAME = "TestSet";
const char* DBVERSION = "1.0";

TestSet::TestSet(QObject *parent)
	: QObject(parent)
{

	opened = false;

	if (!QSqlDatabase::isDriverAvailable("QSQLITE"))
	{
		qWarning("No SQLITE driver available.");
		return;
	}
	QSqlDatabase::addDatabase("QSQLITE", DBNAME);
}

TestSet::~TestSet()
{
}

bool TestSet::create(const QString& path)
{
	QSqlDatabase db = QSqlDatabase::database(DBNAME);
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
	query.bindValue(":db", DBNAME);
	query.bindValue(":version", DBVERSION);
	query.exec();
	query.exec("CREATE TABLE testset ( "
		"id	INTEGER,"
		"name	TEXT,"
		"description	TEXT,"
		"type	INTEGER,"
		"PRIMARY KEY(id)"
		"); ");
	opened = true;
	return true;
}

