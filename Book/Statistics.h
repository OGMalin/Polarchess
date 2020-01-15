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

	hash			text
		position as hashkey.

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
	friend bool operator<(const StatisticsDBMove& m1, const StatisticsDBMove& m2) { return (m2.whitewin + m2.draw + m2.blackwin) < (m1.whitewin + m1.draw + m1.blackwin); };
};

struct StatisticsDBEntry
{
	HASHKEY hash;
	QVector<StatisticsDBMove> movelist;
	StatisticsDBEntry() { clear(); };
	void clear() { hash = 0; movelist.clear(); };
//	bool moveExist(ChessMove& move);
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
	StatisticsDBEntry find(ChessBoard&);

	// Add a move to a position
//	void addMove(StatisticsDBMove& m, ChessBoard& cb);

	// Import pgn-file
	void importGames(QWidget* parent);

	// Remove positions with only one game
	void removeSingleGame(QWidget* parent);

	// Get db path
	QString getPath();

	// Get a list of moves from current board
//	void get(QVector<StatisticsDBMove>&, ChessBoard&);
private:
	bool opened;
	bool haveSingleMove(QString&);
};