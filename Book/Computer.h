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
#include "DatabaseFormat.h"

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

	// Get db path
	QString getPath();

private:
	bool opened;
	ComputerDBEntry lastSearch;
	// Find a position
//	bool find(ComputerDBEntry& sde, ChessBoard& cb);
};