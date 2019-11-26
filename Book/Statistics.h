#pragma once

#include <QObject>
#include <QString>
#include <QVector>
#include <QSqlDatabase>
#include "../Common/ChessMove.h"
#include "../Common/ChessBoard.h"

/*
	Database format

	fen			text
		position as fenstring rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR W kqKQ -

	movelist	text
		move1|win|draw|loss|maxelo|lastyear;move2|win|draw|loss|maxelo|lastyear
*/

struct StatisticsDBMove
{
	ChessMove move;
	int elo;
	int win, draw, loss;
	int year;
	StatisticsDBMove() { clear(); };
	void clear() { move.clear(); win = draw = loss = elo = year = 0; };
};

struct StatisticsDBEntry
{
	ChessBoard board;
	QVector<StatisticsDBMove> movelist;
	bool dirty;
	StatisticsDBEntry() { clear();  dirty = false; };
	void clear() { board.clear(); movelist.clear(); };
	bool moveExist(ChessMove& move);
	void updateMove(StatisticsDBMove&bm);

	void convertToMoveList(QVector<StatisticsDBMove>&, const QString&);
	void convertFromMoveList(const QVector<StatisticsDBMove>&, QString&);
};

struct StatisticsDBInfo
{
	QString db;
	QString version;
};

class Statistics : public QObject
{
public:
	StatisticsDBInfo sdi;
	Statistics(QObject *parent = 0);
	~Statistics();
	bool open(const QString& path);
	bool create(const QString& path);
	void close();
	bool find(StatisticsDBEntry& sde);
	void addMove(StatisticsDBMove& m, ChessBoard& cb);
private:
	bool opened;
};