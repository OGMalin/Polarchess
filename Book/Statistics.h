#pragma once

#include <QObject>
#include <QString>
#include <QVector>
#include <QWidget>
#include <QSqlDatabase>
#include "../Common/ChessMove.h"
#include "../Common/ChessBoard.h"
#include "../Common/Pgn.h"
#include "DatabaseFormat.h"

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
	void importGames2(QWidget* parent);

	// Remove positions with only one game
	void removeSingleGame(QWidget* parent);

	// Get db path
	QString getPath();

	// Get a list of moves from current board
//	void get(QVector<StatisticsDBMove>&, ChessBoard&);
private:
	bool opened;
	bool haveSingleMove(QString&);
	bool readGames(QVector<StatisticsDBEntry>&, Pgn&, int&);
};