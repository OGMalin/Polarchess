#include "Database.h"
#include <QCoreApplication>

Database::Database(QObject *parent)
	: QObject(parent)
{
	QString path = QCoreApplication::applicationDirPath();
	path += "/data/database.db";
	db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName(path);
}

Database::~Database()
{
}

bool Database::create()
{
	return true;
}