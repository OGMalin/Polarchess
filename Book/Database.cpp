#include "Database.h"
#include <QCoreApplication>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QDir>
#include <QVariant>
#include <QSqlError>
#include <QIODevice>
#include <QDebug>
#include "../Common/ChessBoard.h"

const char* DBVERSION = "1.0";
const char* DBTYPE = "BOOKDB";

QDataStream &operator<< (QDataStream& out, const BookDBMove& bdm) { out.writeRawData((const char*)&bdm, sizeof(bdm)); return out; };
QDataStream &operator>> (QDataStream& in, BookDBMove& bdm) { in.readRawData((char*)&bdm, sizeof(bdm)); return in; };



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
	query.exec("CREATE TABLE info ( type TEXT, version TEXT);");
	query.prepare("INSERT INTO info (type, version) VALUES ( :type, :version);");
	query.bindValue(":type", DBTYPE);
	query.bindValue(":version", DBVERSION);
	query.exec();
	query.exec("CREATE TABLE positions ( "
		"fen	TEXT,"
		"opening	TEXT,"
		"variation	TEXT,"
		"subvariation	TEXT,"
		"eco	TEXT,"
		"comment	TEXT,"
		"movelist	BLOB,"
		"PRIMARY KEY(fen)"
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
	bool update = exist(data.fen);

	QSqlQuery query(bookdb);

	if (update)
	{
		query.prepare("UPDATE positions SET "
			"opening = :opening,"
			"variation = :variation,"
			"subvariation = :subvariation,"
			"eco = :eco,"
			"comment = :comment,"
			"movelist = :movelist "
			"WHERE fen = :fen;");
	}
	else
	{
		query.prepare("INSERT INTO positions ( "
			"fen, opening, variation, subvariation, eco, comment, movelist"
			") VALUES ( "
			":fen, :opening, :variation, :subvariation, :eco, :comment, :movelist );");
	}
	query.bindValue(":fen", data.fen.toLatin1());
	query.bindValue(":opening", data.opening.toLatin1());
	query.bindValue(":variation", data.variation.toLatin1());
	query.bindValue(":subvariation", data.subvariation.toLatin1());
	query.bindValue(":eco", data.eco.toLatin1());
	query.bindValue(":comment", data.comment.toLatin1());
	query.bindValue(":movelist", toDBFormat(data.movelist));
	query.exec();
	bookdb.close();
	data.dirty = false;
	return true;
}

BookDBEntry Database::find(QString& fen)
{
	BookDBEntry bde;
	if (!bookdb.open())
		return bde;
	QSqlQuery query(bookdb);
	query.prepare("SELECT * FROM positions WHERE fen = :fen;");
	query.bindValue(":fen", fen.toLatin1());
	if (query.exec() && query.next())
	{
		bde.fen = query.value("fen").toString();
		bde.opening = query.value("opening").toString();
		bde.variation = query.value("variation").toString();
		bde.subvariation = query.value("subvariation").toString();
		bde.eco = query.value("eco").toString();
		bde.comment = query.value("comment").toString();
		bde.movelist = fromDBFormat(query.value("movelist").toByteArray());
		bde.dirty = false;
	}
	bookdb.close();
	return bde;
}

bool Database::exist(QString&fen)
{
	QSqlQuery query;
	query.prepare("SELECT fen FROM positionS WHERE fen = :fen;");
	query.bindValue(":fen", fen.toLatin1());
	if (query.exec() && query.next())
		return true;
	return false;
}

QByteArray Database::toDBFormat(const QVector<BookDBMove>& data)
{
	QByteArray res;
	QDataStream wr(&res, QIODevice::WriteOnly);
	wr << data;
	return res;
}

QVector<BookDBMove> Database::fromDBFormat(const QByteArray& data)
{
	QVector<BookDBMove> res;
	QDataStream re(data);
	re >> res;
	return res;
}

bool BookDBEntry::moveExist(ChessMove& move)
{
	ChessBoard board;
	ChessMove m;
	board.setFen(fen.toStdString().c_str());
	QVector<BookDBMove>::iterator it = movelist.begin();
	while (it != movelist.end())
	{
		board.getMoveFromText(it->cmove.toStdString());
		if (m == move)
			return true;
		++it;
	}
	return false;
}