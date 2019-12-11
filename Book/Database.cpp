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


Database::Database(QString name, QObject *parent)
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
	close();
}

bool Database::open(const QString& path)
{
	QSqlDatabase db = QSqlDatabase::database(dbname);
	db.setDatabaseName(path);

	if (!db.open())
	{
		opened = false;
	}else
	{
		opened = true;
		QSqlQuery query(db);
		query.prepare("SELECT * FROM info;");
		if (query.exec() && query.next())
		{
			bdi.db = query.value("db").toString();
			bdi.version = query.value("version").toString();
			bdi.type = query.value("type").toInt();
		}
		QSqlError error = query.lastError();
		if (error.isValid())
		{
			qDebug() << "Database error: " << error.databaseText();
			qDebug() << "Driver error: " << error.driverText();
		}
	}

	return opened;
}

bool Database::create(const QString& path, int dbtype)
{
	QSqlDatabase db = QSqlDatabase::database(dbname);
	char sz[16];

	db.setDatabaseName(path);
	
	if (!db.open())
	{
		qDebug() << "Create database error. Database: " << db.lastError().databaseText() << "Driver: " << db.lastError().driverText();
		opened = false;
		return false;
	}
	QSqlQuery query(db);
	query.exec("CREATE TABLE info ( db TEXT, version TEXT, type TEXT);");
	QSqlError error = query.lastError();
	if (error.isValid())
	{
		qDebug() << "Database error: " << error.databaseText();
		qDebug() << "Driver error: " << error.driverText();
	}

	query.prepare("INSERT INTO info (db, version, type) VALUES ( :db, :version, :type);");
	query.bindValue(":db", DBTYPE);
	query.bindValue(":version", DBVERSION);
	query.bindValue(":type", itoa(dbtype, sz, 10));
	query.exec();
	error = query.lastError();
	if (error.isValid())
	{
		qDebug() << "Database error: " << error.databaseText();
		qDebug() << "Driver error: " << error.driverText();
	}

	query.exec("CREATE TABLE positions ( "
		"fen	TEXT,"
		"comment	TEXT,"
		"eval	TEXT,"
		"computer	TEXT,"
		"endscore	TEXT,"
		"movelist	TEXT,"
		"PRIMARY KEY(fen)"
		"); ");
	error = query.lastError();
	if (error.isValid())
	{
		qDebug() << "Database error: " << error.databaseText();
		qDebug() << "Driver error: " << error.driverText();
	}

	query.exec("CREATE TABLE training ( "
		"start	TEXT,"
		"endscore	TEXT,"
		"moves	TEXT"
		"); ");
	error = query.lastError();
	if (error.isValid())
	{
		qDebug() << "Database error: " << error.databaseText();
		qDebug() << "Driver error: " << error.driverText();
	}
	query.exec("CREATE INDEX endscore ON training (endscore);");
	error = query.lastError();
	if (error.isValid())
	{
		qDebug() << "Database error: " << error.databaseText();
		qDebug() << "Driver error: " << error.driverText();
	}

	bdi.db = DBTYPE;
	bdi.version = DBVERSION;
	bdi.type = dbtype;
	opened = true;
	return true;
}

void Database::close()
{
	QSqlDatabase db = QSqlDatabase::database(dbname);
	if (db.isOpen())
		db.close();
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
			"comment = :comment,"
			"eval = :eval,"
			"computer = :computer,"
			"endscore = :endscore,"
			"movelist = :movelist "
			"WHERE fen = :fen;");
	}
	else
	{
		query.prepare("INSERT INTO positions ( "
			"fen, comment, eval, computer, endscore, movelist"
			") VALUES ( "
			":fen, :comment, :eval, :computer, :endscore, :movelist );");
	}
	query.bindValue(":fen", bde.board.getFen(true).c_str());
	query.bindValue(":comment", bde.comment);
	query.bindValue(":eval", itoa(bde.eval, sz, 10));
	query.bindValue(":computer", bde.computer);
	query.bindValue(":endscore", itoa(bde.endscore, sz, 10));
	QString qs;
	bde.convertFromMoveList(bde.movelist, qs);
	query.bindValue(":movelist", qs);
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
		bde.comment = query.value("comment").toString();
		bde.eval = query.value("eval").toInt();
		bde.computer = query.value("computer").toString();
		bde.endscore = query.value("endscore").toInt();
		bde.convertToMoveList(bde.movelist, query.value("movelist").toString());
		bde.dirty = false;
	}
	QSqlError error = query.lastError();
	if (error.isValid())
	{
		qDebug() << "Database error: " << error.databaseText();
		qDebug() << "Driver error: " << error.driverText();
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
	QSqlError error = query.lastError();
	if (error.isValid())
	{
		qDebug() << "Database error: " << error.databaseText();
		qDebug() << "Driver error: " << error.driverText();
	}
	return false;
}

BookDBInfo Database::bookInfo()
{
	if (!opened)
	{
		bdi.version.clear();
		bdi.db.clear();
		bdi.type = 0;
	}
	return bdi;
}

void Database::getTrainingPosition(QVector<BookDBEntry>& pos)
{
	int i;
	pos.clear();
	BookDBEntry bde;

	if (!opened)
		return;
	QSqlDatabase db = QSqlDatabase::database(dbname);
	if (!db.open())
		return;
	QSqlQuery query(db);
	query.prepare("SELECT fen,endscore,movelist FROM positions;");
	query.exec();
	QSqlError error = query.lastError();
	if (error.isValid())
	{
		qDebug() << "Database error: " << error.databaseText();
		qDebug() << "Driver error: " << error.driverText();
	}
	while (query.next())
	{
		bde.board.setFen(query.value("fen").toString().toStdString().c_str());
		bde.endscore = query.value("endscore").toInt();
		bde.convertToMoveList(bde.movelist, query.value("movelist").toString());
		pos.push_back(bde);
	}
}

void Database::clearAllTrainingData()
{
	QSqlDatabase db = QSqlDatabase::database(dbname);

	if (!opened)
		return;
	if (!db.open())
		return;

	QSqlQuery query(db);

	query.prepare("UPDATE positions SET "
		"endscore = '0';");
	query.exec();
	QSqlError error = query.lastError();
	if (error.isValid())
	{
		qDebug() << "Database error: " << error.databaseText();
		qDebug() << "Driver error: " << error.driverText();
	}
}

void Database::getRepLines(RepPaths& paths, ChessBoard board, int color, int count)
{
/*
	paths.paths.clear();
	paths.sum = 0;
	if (!opened)
		return;
	QSqlDatabase db = QSqlDatabase::database(dbname);
	if (!db.open())
		return;
	QSqlQuery query(db);
	// Read all positions
	if ((color == 0) || (color == 1))
	{
		query.clear();
		query.prepare("SELECT fen, score, movelist FROM positions WHERE repertoire = :rep;");
		query.bindValue(":rep", 1);
		if (query.exec())
		{
			while (query.next())
			{

			}
		}
		if (query.exec() && query.next())
		{
			bde.opening = query.value("opening").toString();
			bde.eco = query.value("eco").toString();
			bde.comment = query.value("comment").toString();
			bde.eval = query.value("eval").toInt();
			bde.computer = query.value("computer").toString();
			bde.score = query.value("score").toInt();
			bde.repertoire = query.value("repertoire").toInt();
			bde.convertToMoveList(bde.movelist, query.value("movelist").toString());
			bde.dirty = false;
		}
		
	}
*/
	// Read all positions from Database.
}

void Database::importBase(Database* iBase)
{
	BookDBEntry bde1, bde2;
	char  sz[16];
	QString qs;
	QSqlDatabase db1 = QSqlDatabase::database(dbname);
	QSqlDatabase db2 = QSqlDatabase::database(iBase->dbname);
	if (!db1.open())
		return;
	if (!db2.open())
		return;
	QSqlQuery query1(db1);
	QSqlQuery query2(db2);
	query2.prepare("SELECT * FROM positions;");
	query2.exec();
	QSqlError error = query2.lastError();
	if (error.isValid())
	{
		qDebug() << "Database error: " << error.databaseText();
		qDebug() << "Driver error: " << error.driverText();
	}
	while (query2.next())
	{

		bde2.board.setFen(query2.value("fen").toString().toStdString().c_str());
		bde2.comment = query2.value("comment").toString();
		bde2.eval = query2.value("eval").toInt();
		bde2.computer = query2.value("computer").toString();
		bde2.endscore = query2.value("endscore").toInt();
		bde2.convertToMoveList(bde2.movelist, query2.value("movelist").toString());
		bde1.clear();
		bde1.board = bde2.board;
		query1.prepare("SELECT * FROM positions WHERE fen = :fen;");
		query1.bindValue(":fen", bde1.board.getFen(true).c_str());
		if (query1.exec() && query1.next())
		{
			bde1.comment = query1.value("comment").toString();
			bde1.eval = query1.value("eval").toInt();
			bde1.computer = query1.value("computer").toString();
			bde1.endscore = query1.value("endscore").toInt();
			bde1.convertToMoveList(bde1.movelist, query1.value("movelist").toString());
			query1.prepare("UPDATE positions SET "
				"comment = :comment,"
				"eval = :eval,"
				"computer = :computer,"
				"endscore = :endscore,"
				"movelist = :movelist "
				"WHERE fen = :fen;");
		}
		else
		{
			query1.prepare("INSERT INTO positions ( "
				"fen, comment, eval, computer, endscore, movelist"
				") VALUES ( "
				":fen, :comment, :eval, :computer, :endscore, :movelist );");
		}
		bde1.merge(bde2);
		query1.bindValue(":fen", bde1.board.getFen(true).c_str());
		query1.bindValue(":comment", bde1.comment);
		query1.bindValue(":eval", itoa(bde1.eval, sz, 10));
		query1.bindValue(":computer", bde1.computer);
		query1.bindValue(":endscore", itoa(bde1.endscore, sz, 10));
		QString qs;
		bde1.convertFromMoveList(bde1.movelist, qs);
		query1.bindValue(":movelist", qs);
		query1.exec();
		QSqlError error = query1.lastError();
		if (error.isValid())
		{
			qDebug() << "Database error: " << error.databaseText();
			qDebug() << "Driver error: " << error.driverText();
		}
	}
}

void Database::addTrainingLine(QVector<TrainingLine>& tlines)
{
	char sz[16];
	if (!opened)
		return;
	QSqlDatabase db = QSqlDatabase::database(dbname);
	if (!db.open())
		return;
	QSqlQuery query(db);
	query.exec("DELETE FROM training;");
	for (int i = 0; i < tlines.size();i++)
	{
		query.prepare("INSERT INTO training ( "
			"start, endscore, moves"
			") VALUES ( "
			":start, :endscore, :moves );");
		query.bindValue(":start", itoa(tlines[i].start, sz, 10));
		query.bindValue(":endscore", itoa(tlines[i].endscore, sz, 10));
		query.bindValue(":moves", tlines[i].moves);
		query.exec();
		QSqlError error = query.lastError();
		if (error.isValid())
		{
			qDebug() << "Database error: " << error.databaseText();
			qDebug() << "Driver error: " << error.driverText();
		}
	}
}

bool Database::getTrainingLine(TrainingLine& line)
{
	line.moves.clear();
	if (!opened)
		return false;
	QSqlDatabase db = QSqlDatabase::database(dbname);
	if (!db.open())
		return false;
	QSqlQuery query(db);
	query.prepare("SELECT * FROM training ORDERBY endscore;");
	if (query.exec() && query.next())
	{
		line.start = query.value("start").toInt();
		line.endscore = query.value("endscore").toInt();
		line.moves = query.value("moves").toString();
	}
	QSqlError error = query.lastError();
	if (error.isValid())
	{
		qDebug() << "Database error: " << error.databaseText();
		qDebug() << "Driver error: " << error.driverText();
	}
	if (line.moves.isEmpty())
		return false;
	return true;
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
		if (qmove.size() > 0)
			bdm.move = board.getMoveFromText(qmove[0].toStdString());
		if (qmove.size() > 1)
			bdm.comment = qmove[1];
		if (qmove.size() > 2)
			bdm.score = qmove[2].toInt();
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
			data += itoa(it->score, sz, 10);
		}
		++it;
	}
}

void BookDBEntry::updateMove(BookDBMove& bm, bool mergemove)
{
	for (int i = 0; i < movelist.size(); i++)
	{
		if (bm.move == movelist[i].move)
		{
			if (mergemove)
			{
				if (!movelist[i].score)
					movelist[i].score = bm.score;
				if (movelist[i].comment.isEmpty())
					movelist[i].comment = bm.comment;
				return;
			}
			movelist[i] = bm;
			return;
		}
	}
	movelist.append(bm);
}

void BookDBEntry::merge(BookDBEntry& bde)
{
	board = bde.board;
	if (!eval)
		eval = bde.eval;
	if (!bde.computer.isEmpty())
	{
		if (computer != bde.computer)
		{
			if (!computer.isEmpty())
				computer += ";";
			computer += bde.computer;
		}
	}
	if (!bde.comment.isEmpty())
	{
		if (comment != bde.comment)
		{
			if (!comment.isEmpty())
				comment += "\n";
			comment += bde.comment;
		}
	}
	if (!endscore)
		endscore = bde.endscore;
	for (int i = 0; i < bde.movelist.size(); i++)
		updateMove(bde.movelist[i], true);
}

