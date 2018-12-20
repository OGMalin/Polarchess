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
const char* DBTYPE = "POLARBOOKDB";


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
		"eco	TEXT,"
		"comment	TEXT,"
		"movelist	TEXT,"
		"PRIMARY KEY(fen)"
		"); ");
	bookdb.close();
	opened = true;
	return true;
}

bool Database::add(BookDBEntry& bde)
{
	if (!opened)
		return false;
	if (!bookdb.open())
		return false;

	QSqlQuery query(bookdb);

	if (exist(bde.board))
	{
		query.prepare("UPDATE positions SET "
			"opening = :opening,"
			"eco = :eco,"
			"comment = :comment,"
			"movelist = :movelist "
			"WHERE fen = :fen;");
	}
	else
	{
		query.prepare("INSERT INTO positions ( "
			"fen, opening, eco, comment, movelist"
			") VALUES ( "
			":fen, :opening, :eco, :comment, :movelist );");
	}
	query.bindValue(":fen", bde.board.getFen(true).c_str());
	query.bindValue(":opening", bde.opening.toLatin1());
	query.bindValue(":eco", bde.eco.toLatin1());
	query.bindValue(":comment", bde.comment.toLatin1());
	QString qs;
	bde.convertFromMoveList(bde.movelist, qs);
	query.bindValue(":movelist", qs.toLatin1());
	query.exec();
	bookdb.close();
	bde.dirty = false;
	return true;
}

BookDBEntry Database::find(ChessBoard& board)
{
	BookDBEntry bde;

	// Will return current board if it not exist in the db so it don't need to save
	// positions with no other data (comment, moves etc.)
	bde.board = board;
	if (!bookdb.open())
		return bde;
	QSqlQuery query(bookdb);
	query.prepare("SELECT * FROM positions WHERE fen = :fen;");
	query.bindValue(":fen", board.getFen(true).c_str());
	if (query.exec() && query.next())
	{
		bde.board.setFen(query.value("fen").toString().toLatin1());
		bde.opening = query.value("opening").toString();
		bde.eco = query.value("eco").toString();
		bde.comment = query.value("comment").toString();
		bde.convertToMoveList(bde.movelist, query.value("movelist").toString());
		bde.dirty = false;
	}
	bookdb.close();
	return bde;
}

bool Database::exist(ChessBoard& board)
{
	QSqlQuery query;
	query.prepare("SELECT fen FROM positionS WHERE fen = :fen;");
	query.bindValue(":fen", board.getFen(true).c_str());
	if (query.exec() && query.next())
		return true;
	return false;
}

bool BookDBEntry::moveExist(ChessMove& move)
{
	QVector<BookDBMove>::iterator it = movelist.begin();
	while (it != movelist.end())
	{
		if (it->move == move)
			return true;
		++it;
	}
	return false;
}

void BookDBEntry::convertToMoveList(QVector<BookDBMove>& movelist, const QString& data)
{
	BookDBMove bdm;
	QStringList qmove;
	QStringList qlist = data.split(';');

	movelist.clear();
	for (int i = 0; i < qlist.size(); i++)
	{
		qmove = qlist[i].split('|');
		bdm.clear();
		if (qmove.size()>0)
			bdm.move = board.getMoveFromText(qmove[0].toStdString());
		if (qmove.size() > 1)
			bdm.comment = qmove[1];
		if (qmove.size() > 2)
			bdm.repertoire = qmove[2].toInt();
		if (qmove.size() > 3)
			bdm.score = qmove[3].toInt();
		movelist.append(bdm);
	}
}

void BookDBEntry::convertFromMoveList(const QVector<BookDBMove>& movelist, QString& data)
{
	data.clear();
	char sz[16];
	bool first = true;
	QVector<BookDBMove>::ConstIterator it = movelist.begin();
	while (it != movelist.end())
	{
		if (!first)
			data += ";";
		else
			first = false;
		data += board.makeMoveText(it->move, sz, 16, FIDE);
		data += "|";
		data += it->comment;
		data += "|";
		data += itoa(it->repertoire, sz, 10);
		data += "|";
		data += itoa(it->score, sz, 10);
		++it;
	}
}

