#include "Computer.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include "../Common/WinFile.h"

using namespace std;
const char* COMPUTER = "Computer";
const char* CDBVERSION = "1.0";
const char* CDBTYPE = "POLARCOMPUTERDB";

Computer::Computer()
{
	opened = false;
	if (!QSqlDatabase::isDriverAvailable("QSQLITE"))
	{
		qWarning("No SQLITE driver available.");
		return;
	}
	QSqlDatabase::addDatabase("QSQLITE", COMPUTER);
}

Computer::~Computer()
{
}

bool Computer::open(const QString& path)
{
	if (!WinFile::exist(path.toStdString()))
		return false;
	QSqlDatabase db = QSqlDatabase::database(COMPUTER);
	db.setDatabaseName(path);

	if (!db.open())
	{
		opened = false;
	}
	else
	{
		opened = true;
		QSqlQuery query(db);

		query.exec("SELECT * FROM info;");
		if (query.next())
		{
			cdi.db = query.value("db").toString();
			cdi.version = query.value("version").toString();
		}

		query.exec("SELECT * FROM engines;");
		while (query.next())
			enginelist.push_back(query.value("engine").toString());
	}
	lastSearch.clear();
	return opened;
}

bool Computer::create(const QString& path)
{
	QSqlDatabase db = QSqlDatabase::database(COMPUTER);
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
	query.bindValue(":db", CDBTYPE);
	query.bindValue(":version", CDBVERSION);
	query.exec();
	query.exec("CREATE TABLE engines ( engine TEXT);");
	query.exec("CREATE TABLE positions ( "
		"cboard	BLOB, "
		"enginelist	TEXT, "
		"PRIMARY KEY (cboard)"
		" ); ");
	cdi.db = CDBTYPE;
	cdi.version = CDBVERSION;
	lastSearch.clear();
	opened = true;
	return true;
}

void Computer::close()
{
	opened = false;
}

QString Computer::getPath()
{
	QSqlDatabase db = QSqlDatabase::database(COMPUTER);
	return db.databaseName();
}


ComputerDBEntry Computer::find(ChessBoard& cb)
{
	QByteArray cboard = CompressedBoard::compress(cb);
	if (lastSearch.cboard == cboard)
		return lastSearch;

	lastSearch.clear();

	if (!opened)
		return lastSearch;

	QSqlDatabase db = QSqlDatabase::database(COMPUTER);
	if (!db.open())
		return lastSearch;

	QSqlQuery query(db);
	query.prepare("SELECT * FROM positions WHERE cboard = :cboard;");
	query.bindValue(":cboard", cboard);
	if (query.exec() && query.next())
	{
		lastSearch.convertToEngineList(query.value("enginelist").toString(), cb);
		lastSearch.cboard = cboard;
	}
	else
	{
		lastSearch.clear();
	}
	return lastSearch;
}

void Computer::add(ComputerDBEngine& ce, ChessBoard& cb)
{
	bool needUpdate;
	QByteArray cboard;
	char sz[16];
	QSqlError error;

	if (!opened)
		return;

	QSqlDatabase db = QSqlDatabase::database(COMPUTER);
	if (!db.open())
		return;
	QSqlQuery query(db);

	// Check to see if this engine have been used before
	QStringList::iterator eit=enginelist.begin();
	while (eit != enginelist.end())
	{
		if (*eit == ce.engine)
			break;
		++eit;
	}

	// New engine
	if (eit == enginelist.end())
	{
		enginelist.push_back(ce.engine);
		query.prepare("INSERT INTO engines ( engine ) VALUES ( :engine );");
		query.bindValue(":engine", ce.engine);
		query.exec();
		error = query.lastError();
		if (error.isValid())
		{
			qDebug() << "Database error: " << error.databaseText();
			qDebug() << "Driver error: " << error.driverText();
		}
	}

	cboard = CompressedBoard::compress(cb);
	if (lastSearch.cboard != cboard)
	{
		query.prepare("SELECT * FROM positions WHERE cboard = :cboard;");
		query.bindValue(":cboard", cboard);
		if (query.exec() && query.next())
		{
			lastSearch.convertToEngineList(query.value("enginelist").toString(), cb);
			lastSearch.cboard = cboard;
		}
		else
		{
			lastSearch.clear();
		}
	}


	needUpdate = lastSearch.updateEngine(ce);

	if (needUpdate)
	{
		if (lastSearch.cboard != cboard)
		{
			query.prepare("INSERT INTO positions ( "
				"cboard, enginelist"
				") VALUES ( "
				":cboard, :enginelist );");
			lastSearch.cboard = cboard;
		}
		else
		{
			query.prepare("UPDATE positions SET "
				"enginelist = :enginelist "
				"WHERE cboard = :cboard;");
		}
	}
	query.bindValue(":cboard", lastSearch.cboard);
	QString qs;
	lastSearch.convertFromEngineList(qs, cb);
	query.bindValue(":enginelist", qs);
	query.exec();
	error = query.lastError();
	if (error.isValid())
	{
		qDebug() << "Database error: " << error.databaseText();
		qDebug() << "Driver error: " << error.driverText();
	}
	return;
}

void Computer::saveEngineList()
{
	if (!opened)
		return;

	QSqlDatabase db = QSqlDatabase::database(COMPUTER);
	if (!db.open())
		return;
	QSqlQuery query(db);

	query.exec("DELETE FROM engines;");

	QStringList::iterator eit = enginelist.begin();
	db.transaction();
	while (eit != enginelist.end())
	{
		query.prepare("INSERT INTO engines ( engine ) VALUES ( :engine );");
		query.bindValue(":engine", *eit);
		query.exec();
		++eit;
	}
	db.commit();
}
