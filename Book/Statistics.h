#pragma once

#include <QObject>
#include <QString>
#include <QVector>
#include <QWidget>
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
	int whitewin, draw, blackwin;
	int year;
	StatisticsDBMove() { clear(); };
	void clear() { move.clear(); whitewin = draw = blackwin = elo = year = 0; };
};

struct StatisticsDBEntry
{
	HASHKEY hash;
	QVector<StatisticsDBMove> movelist;
	StatisticsDBEntry() { clear(); };
	void clear() { hash = 0; movelist.clear(); };
	bool moveExist(ChessMove& move);
	void updateMove(StatisticsDBMove&bm);

	void convertToMoveList(QVector<StatisticsDBMove>&, const QString&, ChessBoard&);
	void convertFromMoveList(const QVector<StatisticsDBMove>&, QString&, ChessBoard&);
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

	// Open a database
	bool open(const QString& path);

	// Create a database
	bool create(const QString& path);

	// Close the database
	void close();

	// Find a position
	bool find(StatisticsDBEntry& sde, ChessBoard& cb);

	// Add a move to a position
	void addMove(StatisticsDBMove& m, ChessBoard& cb);

	// Import pgn-file
	void importGames(QWidget* parent);

private:
	bool opened;
};