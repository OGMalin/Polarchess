#pragma once
/*
	Database format
	
	fen			text
		position as fenstring rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR W kqKQ -

	comment		text
		Text comment to the position

	eval		text
		Human evaluation

	computer	text
		Computer evaluation

	movescore		text (Only in use on repertoire db)
	endscore		text (Only in use on repertoire db)
		Successfull guesses (-10000=Not tried, -9999= Tried once then starting from 0)

	movelist	text
		move1|comment|score;move2|comment|score
*/
#include <QObject>
#include <QSqlDatabase>
#include <QString>
#include <QVector>
#include <QByteArray>
#include <QDataStream>
#include "../Common/ChessMove.h"
#include "../Common/ChessBoard.h"
#include "Path.h"

struct RepPath
{
	Path path;
	int count;
};

struct RepPaths
{
	QVector<RepPath> paths;
	int sum;
};

struct BookDBMove
{
	ChessMove move;
	QString comment;
	int score=0;
	BookDBMove() { clear(); };
	void clear() { move.clear(); comment.clear(); score = 0; };
};

struct BookDBEntry
{
	ChessBoard board;
	int eval;
	QString computer;
	QString comment;
	int movescore;
	int endscore;
	QVector<BookDBMove> movelist;
	bool dirty;
	BookDBEntry() { clear();  dirty = false; };
	void clear() { board.clear(); comment.clear(); movelist.clear(); computer.clear(); eval = movescore = endscore = 0; dirty = false; };
	bool moveExist(ChessMove& move);
	void updateMove(BookDBMove&bm, bool mergemove=false);

	void convertToMoveList(QVector<BookDBMove>&, const QString&);
	void convertFromMoveList(const QVector<BookDBMove>&, QString&);

	void merge(BookDBEntry& bde);
};

struct BookDBInfo
{
	QString db;
	QString version;
	int type;
};

class Database : public QObject
{
	Q_OBJECT

public:
	BookDBInfo bdi;
	Database(QString name, QObject *parent=0);
	~Database();
	// dbtype: 0=general, 1=White repertoire, 2=Black repertoire
	bool create(const QString& path, int dbtype);
	bool open(const QString& path);
	void close();
	bool add(BookDBEntry& data);
	BookDBEntry find(ChessBoard& board);
	bool isOpen() { return opened; };
	void getRepLines(RepPaths& paths, ChessBoard board, int color, int count);
	BookDBInfo bookInfo();
	void getTrainingPosition(QVector<BookDBEntry>);
	void importBase(Database* iBase);

private:
	bool opened;
//	QSqlDatabase bookdb;
	QString dbname;
	bool exist(ChessBoard& board);
};
