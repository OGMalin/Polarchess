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
		query.prepare("SELECT * FROM info;");
		if (query.exec() && query.next())
		{
			cdi.db = query.value("db").toString();
			cdi.version = query.value("version").toString();
		}
	}

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
	query.exec("CREATE TABLE positions ( "
		"hash	TEXT,"
		"enginelist	TEXT,"
		"PRIMARY KEY(hash)"
		"); ");
	cdi.db = CDBTYPE;
	cdi.version = CDBVERSION;
	opened = true;
	return true;
}

void Computer::close()
{
	opened = false;
}

bool Computer::find(ComputerDBEntry& cde, ChessBoard& cb)
{
	if (!opened)
		return false;
	QSqlDatabase db = QSqlDatabase::database(COMPUTER);
	if (!db.open())
		return false;
	QSqlQuery query(db);
	cde.hash = cb.hashkey();
	query.prepare("SELECT * FROM positions WHERE hash = :hash;");
	query.bindValue(":hash", cde.hash);
	if (query.exec() && query.next())
	{
		cde.convertToEngineList(cde.enginelist, query.value("enginelist").toString(), cb);
		return true;
	}
	return false;
}

void Computer::addEngine(ComputerDBEngine& e, ChessBoard& cb)
{
	QSqlDatabase db = QSqlDatabase::database(COMPUTER);
	ComputerDBEntry cde;
	bool exist;

	char sz[16];
	if (!opened)
		return;
	if (!db.open())
		return;

	QSqlQuery query(db);

	exist = find(cde, cb);

	cde.updateEngine(e);

	if (exist)
	{
		query.prepare("UPDATE positions SET "
			"enginelist = :enginelist "
			"WHERE hash = :hash;");
	}
	else
	{
		query.prepare("INSERT INTO positions ( "
			"hash, enginelist"
			") VALUES ( "
			":hash, :enginelist );");
	}
	query.bindValue(":hash", cde.hash);
	QString qs;
	cde.convertFromEngineList(cde.enginelist, qs, cb);
	query.bindValue(":enginelist", qs);
	query.exec();
	QSqlError error = query.lastError();
	if (error.isValid())
	{
		qDebug() << "Database error: " << error.databaseText();
		qDebug() << "Driver error: " << error.driverText();
	}
	return;
}

void Computer::get(QVector<ComputerDBEngine>&, ChessBoard&)
{

}

bool ComputerDBEntry::engineExist(QString& e)
{
	QVector<ComputerDBEngine>::iterator it = enginelist.begin();
	while (it != enginelist.end())
	{
		if (it->engine == e)
			return true;
		++it;
	}
	return false;
}

void ComputerDBEntry::convertToEngineList(QVector<ComputerDBEngine>& enginelist, const QString& data, ChessBoard& cb)
{
	//ComputerDBEngine cde;
	//QStringList qmove;
	//QStringList qlist = data.split(';');

	//enginelist.clear();
	//for (int i = 0; i < qlist.size(); i++)
	//{
	//	qmove = qlist[i].split('|');
	//	sdm.clear();
	//	if (qmove.size() > 0)
	//		sdm.move = cb.getMoveFromText(qmove[0].toStdString());
	//	if (qmove.size() > 1)
	//		sdm.whitewin = qmove[1].toInt();
	//	if (qmove.size() > 2)
	//		sdm.draw = qmove[2].toInt();
	//	if (qmove.size() > 3)
	//		sdm.blackwin = qmove[3].toInt();
	//	if (qmove.size() > 4)
	//		sdm.elo = qmove[4].toInt();
	//	if (qmove.size() > 5)
	//		sdm.year = qmove[5].toInt();
	//	if (!sdm.move.empty())
	//		movelist.append(sdm);
	//}
}

void ComputerDBEntry::convertFromEngineList(const QVector<ComputerDBEngine>& enginelist, QString& data, ChessBoard& cb)
{
	//data.clear();
	//char sz[16];
	//bool first = true;
	//QVector<StatisticsDBMove>::ConstIterator it = movelist.begin();
	//while (it != movelist.end())
	//{
	//	if (cb.isLegal(ChessMove(it->move)))
	//	{
	//		if (!first)
	//			data += ";";
	//		else
	//			first = false;
	//		data += cb.makeMoveText(it->move, sz, 16, FIDE);
	//		data += "|";
	//		data += itoa(it->whitewin, sz, 10);
	//		data += "|";
	//		data += itoa(it->draw, sz, 10);
	//		data += "|";
	//		data += itoa(it->blackwin, sz, 10);
	//		data += "|";
	//		data += itoa(it->elo, sz, 10);
	//		data += "|";
	//		data += itoa(it->year, sz, 10);
	//	}
	//	++it;
	//}
}

void ComputerDBEntry::updateEngine(ComputerDBEngine& ce)
{
	//for (int i = 0; i < movelist.size(); i++)
	//{
	//	if (bm.move == movelist[i].move)
	//	{
	//		movelist[i].whitewin += bm.whitewin;
	//		movelist[i].draw += bm.draw;
	//		movelist[i].blackwin += bm.blackwin;
	//		movelist[i].elo = __max(movelist[i].elo, bm.elo);
	//		movelist[i].year = __max(movelist[i].year, bm.year);
	//		return;
	//	}
	//}
	//movelist.append(bm);
}

