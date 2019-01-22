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


Database::Database(QString& name, QObject *parent)
	: QObject(parent)
{
	dbname = name;
	opened = false;
	if (!QSqlDatabase::isDriverAvailable("QSQLITE"))
	{
		qWarning("No SQLITE driver available.");
		return;
	}
	QSqlDatabase::addDatabase("QSQLITE", dbname);
}

Database::~Database()
{
}

bool Database::open(const QString& path)
{
	QSqlDatabase db = QSqlDatabase::database(dbname);
	db.setDatabaseName(path);

	if (!db.open())
		opened = false;
	else
		opened = true;
	return opened;
}

bool Database::create(const QString& path)
{
	QSqlDatabase db = QSqlDatabase::database(dbname);

	db.setDatabaseName(path);
	
	if (!db.open())
	{
		qDebug() << "Create database error. Database: " << db.lastError().databaseText() << "Driver: " << db.lastError().driverText();
		opened = false;
		return false;
	}
	QSqlQuery query(db);
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
		"eval	TEXT,"
		"computer	TEXT,"
		"movelist	TEXT,"
		"PRIMARY KEY(fen)"
		"); ");
	opened = true;
	return true;
}

void Database::close()
{
	opened = false;
}

bool Database::add(BookDBEntry& bde)
{
	QSqlDatabase db = QSqlDatabase::database(dbname);

	char sz[16];
	if (!opened)
		return false;
	if (!db.open())
		return false;

	QSqlQuery query(db);

	if (exist(bde.board))
	{
		query.prepare("UPDATE positions SET "
			"opening = :opening,"
			"eco = :eco,"
			"comment = :comment,"
			"eval = :eval,"
			"computer = :computer,"
			"movelist = :movelist "
			"WHERE fen = :fen;");
	}
	else
	{
		query.prepare("INSERT INTO positions ( "
			"fen, opening, eco, comment, eval, computer, movelist"
			") VALUES ( "
			":fen, :opening, :eco, :comment, :eval, :computer, :movelist );");
	}
	query.bindValue(":fen", bde.board.getFen(true).c_str());
	query.bindValue(":opening", bde.opening.toLatin1());
	query.bindValue(":eco", bde.eco.toLatin1());
	query.bindValue(":comment", bde.comment.toLatin1());
	query.bindValue(":eval", itoa(bde.eval, sz, 10));
	query.bindValue(":computer", bde.computer.toLatin1());
	QString qs;
	bde.convertFromMoveList(bde.movelist, qs);
	query.bindValue(":movelist", qs.toLatin1());
	query.exec();
	QSqlError error = query.lastError();
	if (error.isValid())
	{
		qDebug() << "Database error: " << error.databaseText();
		qDebug() << "Driver error: " << error.driverText();
	}
	bde.dirty = false;
	return true;
}

BookDBEntry Database::find(ChessBoard& board)
{
	BookDBEntry bde;

	// Will return current board if it not exist in the db so it don't need to save
	// positions with no other data (comment, moves etc.)
	bde.board = board;
	if (!opened)
		return bde;
	QSqlDatabase db = QSqlDatabase::database(dbname);
	if (!db.open())
		return bde;
	QSqlQuery query(db);
	query.prepare("SELECT * FROM positions WHERE fen = :fen;");
	query.bindValue(":fen", board.getFen(true).c_str());
	if (query.exec() && query.next())
	{
		bde.board.setFen(query.value("fen").toString().toLatin1());
		bde.opening = query.value("opening").toString();
		bde.eco = query.value("eco").toString();
		bde.comment = query.value("comment").toString();
		bde.eval = query.value("eval").toInt();
		bde.computer = query.value("computer").toString();
		bde.convertToMoveList(bde.movelist, query.value("movelist").toString());
		bde.dirty = false;
	}
	return bde;
}

bool Database::exist(ChessBoard& board)
{
	QSqlDatabase db = QSqlDatabase::database(dbname);
	QSqlQuery query(db);
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
		if (!bdm.move.empty())
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
		if (board.isLegal(ChessMove(it->move)))
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
		}
		++it;
	}
}

