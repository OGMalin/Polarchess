#include "Openings.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include "../Common/WinFile.h"

const char* OPENINGS = "Openings";
const char* ODBVERSION = "1.0";
const char* ODBTYPE = "POLAROPENINGSDB";

Openings::Openings()
{
	_opened = false;
	if (!QSqlDatabase::isDriverAvailable("QSQLITE"))
	{
		qWarning("No SQLITE driver available.");
		return;
	}
	QSqlDatabase::addDatabase("QSQLITE", OPENINGS);
}

Openings::~Openings()
{
	close();
}

bool Openings::open(const QString& path)
{
	if (!WinFile::exist(path.toStdString()))
		return false;

	QSqlDatabase db = QSqlDatabase::database(OPENINGS);
	db.setDatabaseName(path);

	if (!db.open())
	{
		_opened = false;
	}
	else
	{
		_opened = true;
		QSqlQuery query(db);

		query.exec("SELECT * FROM info;");
		if (query.next())
		{
			odi.db = query.value("db").toString();
			odi.version = query.value("version").toString();
		}

	}
	return _opened;
}

bool Openings::create(const QString& path)
{
	QSqlDatabase db = QSqlDatabase::database(OPENINGS);
	char sz[16];

	db.setDatabaseName(path);

	if (!db.open())
	{
		qDebug() << "Create database error. Database: " << db.lastError().databaseText() << "Driver: " << db.lastError().driverText();
		_opened = false;
		return false;
	}
	QSqlQuery query(db);
	query.exec("CREATE TABLE info ( db TEXT, version TEXT);");
	query.prepare("INSERT INTO info (db, version) VALUES ( :db, :version);");
	query.bindValue(":db", ODBTYPE);
	query.bindValue(":version", ODBVERSION);
	query.exec();
	query.exec("CREATE TABLE openings ( "
		"position BLOB,"
		"eco	TEXT,"
		"name	TEXT,"
		"variation	TEXT,"
		"subvariation	TEXT"
		"); ");
	query.exec("CREATE INDEX position on training (position);");
	odi.db = ODBTYPE;
	odi.version = ODBVERSION;
	_opened = true;
	return true;
}

void Openings::close()
{
	_opened = false;
}

QString Openings::getPath()
{
	QSqlDatabase db = QSqlDatabase::database(OPENINGS);
	return db.databaseName();
}


OpeningsDBEntry Openings::find(ChessBoard& cb)
{
	OpeningsDBEntry ode;

	if (!_opened)
		return ode;

	QSqlDatabase db = QSqlDatabase::database(OPENINGS);
	if (!db.open())
		return ode;

	ode.cBoard = CompressedBoard::compress(cb);
	QSqlQuery query(db);
	query.prepare("SELECT * FROM openings WHERE position = :position;");
	query.bindValue(":position", ode.cBoard);
	if (query.exec() && query.next())
	{
		ode.eco = query.value("eco").toString();
		ode.name = query.value("name").toString();
		ode.variation = query.value("variation").toString();
		ode.subvariation = query.value("subvariation").toString();
	}
	else
	{
		ode.clear();
	}
	return ode;
}

void Openings::add(OpeningsDBEntry& ode, ChessBoard& cb)
{
	QSqlError error;

	if (!_opened)
		return;

	QSqlDatabase db = QSqlDatabase::database(OPENINGS);
	if (!db.open())
		return;
	QSqlQuery query(db);

	if (ode.cBoard.size() == 0)
	{
		ode.cBoard = CompressedBoard::compress(cb);
		query.prepare("INSERT INTO openings ( "
			"position, eco, name, variation, subvariation"
			" ) VALUES ( "
			":position, :eco, :name, :variation, :subvariation );");
	}
	else
	{
		query.prepare("UPDATE openings SET "
			"eco = :eco, "
			"name = :name, "
			"variation = :variation, "
			"subvariation = :subvariation "
			"WHERE position = :position;");
	}
	query.bindValue(":eco", ode.eco);
	query.bindValue(":name", ode.name);
	query.bindValue(":variation", ode.variation);
	query.bindValue(":subvariation", ode.subvariation);
	query.bindValue(":position", ode.cBoard);
	query.exec();
	error = query.lastError();
	if (error.isValid())
	{
		qDebug() << "Database error: " << error.databaseText();
		qDebug() << "Driver error: " << error.driverText();
	}
	return;
}