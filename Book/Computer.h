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

	enginelist	text
		engine1|cp|depth|pv;engine2|cb|depth|pv
*/

struct ComputerDBEngine
{
	QString engine;
	int cp;
	int depth;
	QVector<ChessMove> pv;
	ComputerDBEngine() { clear(); };
	void clear() { engine.clear(); cp = depth = 0; pv.clear(); };
};

struct ComputerDBEntry
{
	HASHKEY hash;
	QVector<ComputerDBEngine> enginelist;
	ComputerDBEntry() { clear(); };
	void clear() { hash = 0; enginelist.clear(); };
	bool engineExist(QString& e);
	void updateEngine(ComputerDBEngine&bm);

	void convertToEngineList(QVector<ComputerDBEngine>&, const QString&, ChessBoard&);
	void convertFromEngineList(const QVector<ComputerDBEngine>&, QString&, ChessBoard&);
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
	QVector<QString> enginelist;

	Computer();
	~Computer();

	// Open a database
	bool open(const QString& path);

	// Create a database
	bool create(const QString& path);

	// Close the database
	void close();

	// Find a position
	bool find(ComputerDBEntry& sde, ChessBoard& cb);

	// Add an engine to a position
	void addEngine(ComputerDBEngine& c, ChessBoard& cb);

	// Get a list of engine analysis from current board
	void get(QVector<ComputerDBEngine>&, ChessBoard&);

private:
	bool opened;
};