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
		"hash	TEXT,"
		"enginelist	TEXT,"
		"PRIMARY KEY(hash)"
		"); ");
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

ComputerDBEntry Computer::find(ChessBoard& cb)
{
	HASHKEY hash = cb.hashkey();
	if (lastSearch.hash == hash)
		return lastSearch;

	lastSearch.clear();

	if (!opened)
		return lastSearch;

	QSqlDatabase db = QSqlDatabase::database(COMPUTER);
	if (!db.open())
		return lastSearch;

	QSqlQuery query(db);
	query.prepare("SELECT * FROM positions WHERE hash = :hash;");
	query.bindValue(":hash", hash);
	if (query.exec() && query.next())
	{
		lastSearch.convertToEngineList(query.value("enginelist").toString(), cb);
		lastSearch.hash = hash;
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
	HASHKEY hash;
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

	hash = cb.hashkey();
	if (lastSearch.hash != hash)
	{
		query.prepare("SELECT * FROM positions WHERE hash = :hash;");
		query.bindValue(":hash", hash);
		if (query.exec() && query.next())
		{
			lastSearch.convertToEngineList(query.value("enginelist").toString(), cb);
			lastSearch.hash = hash;
		}
		else
		{
			lastSearch.clear();
		}
	}


	needUpdate = lastSearch.updateEngine(ce);

	if (needUpdate)
	{
		if (lastSearch.hash != hash)
		{
			query.prepare("INSERT INTO positions ( "
				"hash, enginelist"
				") VALUES ( "
				":hash, :enginelist );");
			lastSearch.hash = hash;
		}
		else
		{
			query.prepare("UPDATE positions SET "
				"enginelist = :enginelist "
				"WHERE hash = :hash;");
		}
	}
	query.bindValue(":hash", lastSearch.hash);
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

void ComputerDBEntry::convertToEngineList(const QString& data, ChessBoard& cb)
{
	//	engine1 | cp | depth | time | pv; engine2 | cb | depth | time | pv
	
	QStringList qlist = data.split(';');
	QStringList entry;
	QStringList smoves;
	ComputerDBEngine ce;
	ChessBoard b;
	ChessMove m;
	QStringList::iterator mit;
	enginelist.clear();

	QStringList::iterator it = qlist.begin();
	while (it != qlist.end())
	{
		ce.clear();
		entry = it->split('|');
		if (entry.size() > 0)
			ce.engine = entry[0];
		if (entry.size() > 1)
			ce.cp = entry[1].toInt();
		if (entry.size() > 2)
			ce.depth = entry[2].toInt();
		if (entry.size() > 3)
			ce.time = entry[3].toInt();
		if (entry.size() > 4)
		{
			smoves = entry[4].split(' ');
			mit = smoves.begin();
			b = cb;
			while (mit != smoves.end())
			{
				m = b.getMoveFromText(mit->toStdString());
				ce.pv.push_back(m);
				b.doMove(m, false);
				++mit;
			}
		}
		enginelist.push_back(ce);
		++it;
	}
}

void ComputerDBEntry::convertFromEngineList(QString& data, ChessBoard& cb)
{
//	engine1 | cp | depth | time | pv; engine2 | cb | depth | time | pv
	data.clear();
	char sz[16];
	string s;
	ChessBoard b;
	int mit;
	QVector<ComputerDBEngine>::iterator it = enginelist.begin();
	while (it != enginelist.end())
	{
		if (it!=enginelist.begin())
			data += ";";
		data += it->engine;
		data += "|";
		data += itoa(it->cp, sz, 10);
		data += "|";
		data += itoa(it->depth, sz, 10);
		data += "|";
		data += itoa(it->time, sz, 10);
		data += "|";
		b = cb;
		for (mit = 0;mit<it->pv.size;mit)
		{
			s=b.makeMoveText(it->pv.list[mit], FIDE);
			if (mit>0)
				data += " ";
			data += s.c_str();
			b.doMove(it->pv.list[mit], false);
			++mit;
		}
		++it;
	}
}

bool ComputerDBEntry::updateEngine(ComputerDBEngine& ce)
{
	QVector<ComputerDBEngine>::iterator it = enginelist.begin();
	while (it != enginelist.end())
	{
		if (it->engine == ce.engine)
		{
			if ((ce.depth>it->depth) || (ce.time > it->time) && (ce.depth == it->depth))
			{
				*it = ce;
				return true;
			}
			else
			{
				return false;
			}
		}
		++it;
	}
	enginelist.push_back(ce);
	return true;
}

