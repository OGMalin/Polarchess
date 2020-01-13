#include "Database.h"
#include "../Common/WinFile.h"
#include <QCoreApplication>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QDir>
#include <QVariant>
#include <QSqlError>
#include <QIODevice>
#include <QVariantList>
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
	if (!WinFile::exist(path.toStdString()))
		return false;
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
			opened = false;
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
		"score	TEXT,"
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
		"score	TEXT,"
		"moves	TEXT"
		"); ");
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
			"score = :score,"
			"movelist = :movelist "
			"WHERE fen = :fen;");
	}
	else
	{
		query.prepare("INSERT INTO positions ( "
			"fen, comment, eval, score, movelist"
			") VALUES ( "
			":fen, :comment, :eval, :score, :movelist );");
	}
	query.bindValue(":fen", bde.board.getFen(true).c_str());
	query.bindValue(":comment", bde.comment);
	query.bindValue(":eval", itoa(bde.eval, sz, 10));
	query.bindValue(":score", itoa(bde.score, sz, 10));
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
		bde.score = query.value("score").toInt();
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
	query.prepare("SELECT fen,score,movelist FROM positions;");
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
		bde.score = query.value("score").toInt();
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

	query.prepare("UPDATE positions SET score = '0';");
	query.exec();
	QSqlError error = query.lastError();
	if (error.isValid())
	{
		qDebug() << "Database error: " << error.databaseText();
		qDebug() << "Driver error: " << error.driverText();
	}
	deleteTrainingLines();
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
		bde2.score = query2.value("score").toInt();
		bde2.convertToMoveList(bde2.movelist, query2.value("movelist").toString());
		bde1.clear();
		bde1.board = bde2.board;
		query1.prepare("SELECT * FROM positions WHERE fen = :fen;");
		query1.bindValue(":fen", bde1.board.getFen(true).c_str());
		if (query1.exec() && query1.next())
		{
			bde1.comment = query1.value("comment").toString();
			bde1.eval = query1.value("eval").toInt();
			bde1.score = query1.value("score").toInt();
			bde1.convertToMoveList(bde1.movelist, query1.value("movelist").toString());
			query1.prepare("UPDATE positions SET "
				"comment = :comment,"
				"eval = :eval,"
				"score = :score,"
				"movelist = :movelist "
				"WHERE fen = :fen;");
		}
		else
		{
			query1.prepare("INSERT INTO positions ( "
				"fen, comment, eval, score, movelist"
				") VALUES ( "
				":fen, :comment, :eval, :score, :movelist );");
		}
		bde1.merge(bde2);
		query1.bindValue(":fen", bde1.board.getFen(true).c_str());
		query1.bindValue(":comment", bde1.comment);
		query1.bindValue(":eval", itoa(bde1.eval, sz, 10));
		query1.bindValue(":score", itoa(bde1.score, sz, 10));
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

void Database::addTrainingLines(QVector<TrainingLine>& tlines)
{
	char sz[16];
	if (!opened)
		return;
	QSqlDatabase db = QSqlDatabase::database(dbname);
	if (!db.open())
		return;
	QSqlQuery query(db);
	QVariantList score, moves;
	for (int i = 0; i < tlines.size();i++)
	{
		score.push_back(itoa(tlines[i].score, sz, 10));
		moves.push_back(tlines[i].moves);
	}
	if (score.size() != moves.size())
	{
		qDebug() << "Score and moves missmatch";
		return;
	}
	int next = 0;
	int i;
	while (next < score.size())
	{
		db.transaction();
		for (i = 0; i < 100; i++)
		{
			if (next >= score.size())
				break;
			query.prepare("INSERT INTO training ( score, moves ) VALUES ( :score, :moves );");
			query.bindValue(":score", score[next]);
			query.bindValue(":moves", moves[next]);
			query.exec();
			++next;
		}
		db.commit();
	}
}

void Database::deleteTrainingLines()
{
	if (!opened)
		return;
	QSqlDatabase db = QSqlDatabase::database(dbname);
	if (!db.open())
		return;
	QSqlQuery query(db);
	query.exec("DELETE FROM training;");
	QSqlError error = query.lastError();
	if (error.isValid())
	{
		qDebug() << "Database error: " << error.databaseText();
		qDebug() << "Driver error: " << error.driverText();
	}
}

bool Database::getTrainingLines(QVector<TrainingLine>& lines)
{
	TrainingLine tl;
	lines.clear();
	if (!opened)
		return false;
	QSqlDatabase db = QSqlDatabase::database(dbname);
	if (!db.open())
		return false;
	QSqlQuery query(db);
	query.prepare("SELECT rowid, * FROM training;");
	query.exec();
	while (query.next())
	{
		tl.rowid = query.value("rowid").toInt();
		tl.score = query.value("score").toInt();
		tl.moves = query.value("moves").toString();
		lines.push_back(tl);
	}
	QSqlError error = query.lastError();
	if (error.isValid())
	{
		qDebug() << "Database error: " << error.databaseText();
		qDebug() << "Driver error: " << error.driverText();
	}
	if (lines.isEmpty())
		return false;
	return true;
}

void Database::updateTrainingScore(ChessBoard& cb, int rowid, int score)
{
	char sz[16];
	if (!opened)
		return;
	QSqlDatabase db = QSqlDatabase::database(dbname);
	if (!db.open())
		return;
	QSqlQuery query(db);
	query.prepare("UPDATE positions SET score = :score WHERE fen = :fen;");
	query.bindValue(":fen", cb.getFen(true).c_str());
	query.bindValue(":score", itoa(score, sz, 10));
	query.exec();
	query.prepare("UPDATE training SET score = :score WHERE rowid = :rowid;");
	query.bindValue(":score", itoa(score, sz, 10));
	query.bindValue(":rowid", itoa(rowid, sz, 10));
	query.exec();
}

bool BookDBEntry::moveExist(const ChessMove& move)
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

void BookDBEntry::deleteMove(const ChessMove& move)
{
	int i;
	for (i = 0; i < movelist.size(); i++)
	{
		if (movelist[i].move == move)
		{
			movelist.remove(i);
			return;
		}
	}
}

void BookDBEntry::setFirst(const ChessMove& move)
{
	int i;
	for (i = 0; i < movelist.size(); i++)
		if (movelist[i].move == move)
			break;
	if (i == movelist.size()) // Not found
		return;
	if (i == 0) // Allready first
		return;
	BookDBMove bm = movelist[0];
	movelist[0] = movelist[i];
	movelist[i] = bm;
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
	if (!bde.comment.isEmpty())
	{
		if (comment != bde.comment)
		{
			if (!comment.isEmpty())
				comment += "\n";
			comment += bde.comment;
		}
	}
	if (!score)
		score = bde.score;
	for (int i = 0; i < bde.movelist.size(); i++)
		updateMove(bde.movelist[i], true);
}

