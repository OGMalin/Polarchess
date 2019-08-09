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

	score		text (Only in use on repertoire db)
		Successfull guesses (-10000=Not tried, -9999= Tried once then starting from 0)

	repertoire	text (Only in use on repertoire db)
		0=White, 1=black

	bookmark	text
		Mark position (training)

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
	int repertoire; // 1= White, 2=Black, 0=none
	QString computer;
	QString comment;
	QString bookmark;
	int score=0;
	QVector<BookDBMove> movelist;
	bool dirty;
	BookDBEntry() { clear();  dirty = false; };
	void clear() { board.clear(); comment.clear(); movelist.clear(); computer.clear(); eval = 0; repertoire = 0; dirty = false; };
	bool moveExist(ChessMove& move);

	void convertToMoveList(QVector<BookDBMove>&, const QString&);
	void convertFromMoveList(const QVector<BookDBMove>&, QString&);
};

class Database : public QObject
{
	Q_OBJECT

public:
	Database(QString& name, QObject *parent=0);
	~Database();
	bool create(const QString& path);
	bool open(const QString& path);
	void close();
	bool add(BookDBEntry& data);
	BookDBEntry find(ChessBoard& board, int rep);
	bool isOpen() { return opened; };
	void getRepLines(RepPaths& paths, ChessBoard board, int color, int count);

private:
	bool opened;
//	QSqlDatabase bookdb;
	QString dbname;
	bool exist(ChessBoard& board, int rep);
};
