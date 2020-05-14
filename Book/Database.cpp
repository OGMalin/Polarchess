#include "Database.h"
#include "Training.h"
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
		"cboard	BLOB, "
		"comment	TEXT, "
		"eval	TEXT, "
		"attempt	TEXT, "
		"score	TEXT, "
		"movelist	TEXT, "
		"PRIMARY KEY (cboard) "
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
			"attempt = :attempt"
			"score = :score,"
			"movelist = :movelist "
			"WHERE cboard = :cboard;");
	}
	else
	{
		query.prepare("INSERT INTO positions ( "
			"cboard, comment, eval, attempt, score, movelist"
			") VALUES ( "
			":cboard, :comment, :eval, :attempt, :score, :movelist );");
	}
	query.bindValue(":cboard", CompressedBoard::compress(bde.board));
	query.bindValue(":comment", bde.comment);
	query.bindValue(":eval", itoa(bde.eval, sz, 10));
	query.bindValue(":attempt", itoa(bde.attempt, sz, 10));
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
	query.prepare("SELECT * FROM positions WHERE cboard = :cboard;");
	query.bindValue(":cboard", CompressedBoard::compress(board));
	if (query.exec() && query.next())
	{
		bde.comment = query.value("comment").toString();
		bde.eval = query.value("eval").toInt();
		bde.attempt = query.value("attempt").toInt();
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
	query.prepare("SELECT cboard FROM positionS WHERE cboard = :cboard;");
	query.bindValue(":cboard", CompressedBoard::compress(board));
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
	query.prepare("SELECT cboard, attempt, score, movelist FROM positions;");
	query.exec();
	QSqlError error = query.lastError();
	if (error.isValid())
	{
		qDebug() << "Database error: " << error.databaseText();
		qDebug() << "Driver error: " << error.driverText();
	}
	while (query.next())
	{
		bde.board = CompressedBoard::decompress(query.value("cboard").toByteArray());
		bde.attempt = query.value("attempt").toInt();
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

	query.prepare("UPDATE positions SET score = '0', attempt = '0';");
	query.exec();
	QSqlError error = query.lastError();
	if (error.isValid())
	{
		qDebug() << "Database error: " << error.databaseText();
		qDebug() << "Driver error: " << error.driverText();
	}
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

		bde2.board = CompressedBoard::decompress(query2.value("cboard").toByteArray());
		bde2.comment = query2.value("comment").toString();
		bde2.eval = query2.value("eval").toInt();
		bde2.attempt = query2.value("attempt").toInt();
		bde2.score = query2.value("score").toInt();
		bde2.convertToMoveList(bde2.movelist, query2.value("movelist").toString());
		bde1.clear();
		bde1.board = bde2.board;
		query1.prepare("SELECT * FROM positions WHERE cboard = :cboard;");
		query1.bindValue(":cboard", CompressedBoard::compress(bde1.board));
		if (query1.exec() && query1.next())
		{
			bde1.comment = query1.value("comment").toString();
			bde1.eval = query1.value("eval").toInt();
			bde1.attempt = query1.value("attempt").toInt();
			bde1.score = query1.value("score").toInt();
			bde1.convertToMoveList(bde1.movelist, query1.value("movelist").toString());
			query1.prepare("UPDATE positions SET "
				"comment = :comment,"
				"eval = :eval,"
				"attempt = :attempt,"
				"score = :score,"
				"movelist = :movelist "
				"WHERE cboard = :cboard;");
		}
		else
		{
			query1.prepare("INSERT INTO positions ( "
				"cboard, comment, eval, attempt, score, movelist"
				") VALUES ( "
				":cboard, :comment, :eval, :attempt, :score, :movelist );");
		}
		bde1.merge(bde2);
		query1.bindValue(":cboard", CompressedBoard::compress(bde1.board));
		query1.bindValue(":comment", bde1.comment);
		query1.bindValue(":eval", itoa(bde1.eval, sz, 10));
		query1.bindValue(":attempt", itoa(bde1.attempt, sz, 10));
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

void Database::updateTrainingScore(TrainingLine& tl)
{
	int i;
	ChessBoard cb;
	char sz[16];
	if (!opened)
		return;
	QSqlDatabase db = QSqlDatabase::database(dbname);
	if (!db.open())
		return;
	QSqlQuery query(db);
	cb.setStartposition();
	for (i = 0; i < tl.moves.size(); i++)
	{
		if (tl.moves[i].attempt || tl.moves[i].score)
		{
			query.prepare("UPDATE positions SET attempt = attempt + :attempt, score = score + :score WHERE cboard = :cboard;");
			query.bindValue(":cboard", CompressedBoard::compress(cb));
			query.bindValue(":attempt", itoa(tl.moves[i].attempt, sz, 10));
			query.bindValue(":score", itoa(tl.moves[i].score, sz, 10));
			query.exec();
		}
		cb.doMove(tl.moves[i].move, false);
	}
}

QString Database::getPath()
{
	QSqlDatabase db = QSqlDatabase::database(dbname);
	return db.databaseName();
}

void Database::getAllPositions(QVector<ChessBoard>& pos)
{
	if (!opened)
		return;
	QSqlDatabase db = QSqlDatabase::database(dbname);
	if (!db.open())
		return;
	QSqlQuery query(db);
	query.exec("SELECT cboard FROM positions;");
	while (query.next())
		pos.push_back(CompressedBoard::decompress(query.value("cboard").toByteArray()));
}

void Database::getEndPositions(QVector<ChessBoard>& pos)
{
	QVector<BookDBEntry> bdes;
	BookDBEntry bde;
	MoveList path;
	QVector<MoveList> pathlist;
	ChessBoard cb;
	int i, j;

	if (!opened)
		return;
	QSqlDatabase db = QSqlDatabase::database(dbname);
	if (!db.open())
		return;
	QSqlQuery query(db);

	// Collect all positions
	query.prepare("SELECT cboard, movelist FROM positions;");
	query.exec();
	QSqlError error = query.lastError();
	if (error.isValid())
	{
		qDebug() << "Database error: " << error.databaseText();
		qDebug() << "Driver error: " << error.driverText();
		return;
	}
	while (query.next())
	{
		bde.board = CompressedBoard::decompress(query.value("cboard").toByteArray());
		bde.convertToMoveList(bde.movelist, query.value("movelist").toString());
		bdes.push_back(bde);
	}
	if (bdes.size() == 0)
		return;

	// Create a tree
	std::sort(bdes.begin(), bdes.end());
	cb.setStartposition();
	walkThrough(cb, path, 0, bdes, pathlist);

	// Find end positions
	pos.clear();
	for (i = 0; i < pathlist.size(); i++)
	{
		cb.setStartposition();
		for (j = 0; j < pathlist[i].size(); j++)
			cb.doMove(pathlist[i][j], false);
		if (j > 0)
			pos.push_back(cb);
	}
}

void Database::walkThrough(ChessBoard& cb, MoveList& path, int ply, QVector<BookDBEntry>& pos, QVector<MoveList>& pathlist)
{
	int  curmove = 0;
	BookDBEntry bde;
	QVector<BookDBEntry>::iterator bid;
//	ChessMove move;

	// Get position 
	bde.board = cb;
	bid = std::lower_bound(pos.begin(), pos.end(), bde);

	// The position don't exist or no moves or repeating move
	if ((bid->board != cb) || (bid->movelist.size() == 0))
	{
		pathlist.push_back(path);
		return;
	}

	// Skip this line because it doesn't have an endposition
	if (bid->dirty)
		return;

	bid->dirty = true;

	while (1)
	{
//		move = bid->movelist[curmove].move;
		path.push_back(bid->movelist[curmove].move);
		cb.doMove(bid->movelist[curmove].move, false);
		walkThrough(cb, path, ply + 1, pos, pathlist);
		path.pop_back();
		++curmove;
		if (curmove >= bid->movelist.size())
			break;
		cb = bde.board;
	}
}

#ifdef _DEBUG
void Database::convertBase()
{
	struct DBRec
	{
		int rowid;
		ChessBoard board;
	};
	QVector<DBRec> rec;
	QSqlDatabase db = QSqlDatabase::database(dbname);
	if (!db.open())
		return;
	QSqlQuery query(db);

	// Rename coulumn
	db.transaction();
	query.exec("CREATE TABLE temppos ( "
		"cboard	BLOB, "
		"comment	TEXT, "
		"eval	TEXT, "
		"attempt	TEXT, "
		"score	TEXT, "
		"movelist	TEXT, "
		"PRIMARY KEY (cboard) "
		"); ");
	
	query.exec("INSERT INTO temppos (cboard, comment, eval, attempt, score, movelist) SELECT fen, comment, eval, attempt, score, movelist FROM positions;");
	query.exec("DROP TABLE positions;");
	query.exec("ALTER TABLE temppos RENAME TO positions;");
	db.commit();

	query.exec("SELECT rowid, cboard FROM positions;");
	DBRec r;
	while (query.next())
	{
		r.rowid = query.value("rowid").toInt();
		r.board.setFen(query.value("cboard").toString().toStdString().c_str());
		rec.push_back(r);
	}
	db.transaction();
	for (int i = 0; i < rec.size(); i++)
	{
		query.prepare("UPDATE positions SET cboard = :cboard WHERE rowid = :rowid;");
		query.bindValue(":cboard", CompressedBoard::compress(rec[i].board));
		query.bindValue(":rowid", rec[i].rowid);
		query.exec();
	}
	db.commit();
	query.exec("VACUUM;");
}
#endif