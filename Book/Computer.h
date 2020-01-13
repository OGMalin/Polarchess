#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QWidget>
#include <QSqlDatabase>
#include "../Common/ChessMove.h"
#include "../Common/ChessBoard.h"
#include "../Common/MoveList.h"

/*
	Database format

	hash			text
		position as hashkey.

	enginelist	text
		engine1|cp|depth|time|pv;engine2|cb|depth|time|pv
*/

struct ComputerDBEngine
{
	QString engine;
	int cp;
	int depth;
	int time;
	MoveList pv;
	ComputerDBEngine() { clear(); };
	void clear() { engine.clear(); cp = depth = 0; pv.clear(); };
};

struct ComputerDBEntry
{
	HASHKEY hash;
	QVector<ComputerDBEngine> enginelist;
	ComputerDBEntry() { clear(); };
	void clear() { hash = 0; enginelist.clear(); };
//	bool engineExist(QString& e);
	bool updateEngine(ComputerDBEngine&);

	void convertToEngineList(const QString&, ChessBoard&);
	void convertFromEngineList(QString&, ChessBoard&);
};

struct ComputerDBInfo
{
	QString db;
	QString version;
};

class Computer : public QObject
{
public:
	ComputerDBInfo cdi;
	// List of all engines with evaluation in this db.
	QStringList enginelist;

	Computer();
	~Computer();

	// Open a database
	bool open(const QString& path);

	// Create a database
	bool create(const QString& path);

	// Close the database
	void close();

	// Add an engine analyze to a position
	void add(ComputerDBEngine& c, ChessBoard& cb);

	// Find a position
	ComputerDBEntry find(ChessBoard&);

	// Save the engine priority list
	void saveEngineList();

private:
	bool opened;
	ComputerDBEntry lastSearch;
	// Find a position
//	bool find(ComputerDBEntry& sde, ChessBoard& cb);
};