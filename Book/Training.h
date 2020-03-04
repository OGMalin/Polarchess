#pragma once
#include "DatabaseFormat.h"
#include "Database.h"
#include "../Common/ChessBoard.h"
#include "../Common/ChessMove.h"
#include "../Common/MoveList.h"
#include <QString>
#include <QVector>
#include <QWidget>
#include <QSqlDatabase>

class Training
{
private:
	TrainingDBInfo tdi;
	TrainingStatistics stat;
	QVector<TrainingDBEntry> list;
//	QVector<TrainingDBEntry> currentList;
//	ChessBoard currentBoard;
//	int currentColor;
	void walkThrough(ChessBoard& cur, TrainingDBEntry& path, int ply, QVector<BookDBEntry>& pos, int color);
//	void convertMovesFromString(const QString& smoves, TrainingDBEntry& tp);
//	void convertMovesToString(QString& smoves, TrainingDBEntry& tp);
	bool opened;
public:
	Database* Base[2];

	Training();
	~Training();

	// Open a database
	bool open(const QString& path);

	// Create a database
	bool create(const QString& path);

	// Close the database
	void close();

	// Set the database to use (color=WHITE or BLACK)
	void SetRepertoireDatabase(int color, Database* base);

	// Removing all trainingdata and statistics
	void clearAll();

	// Create traininglines. 
	void createLines(QWidget* parent);

	// Get next training line
	bool getNext(TrainingDBEntry& line, int color, ChessBoard& cb);

	// Update training score
	void updateScore(TrainingDBEntry&);

	// Get db path
	QString getPath();

	TrainingStatistics getStat();

	void getEndpositions(QVector<ChessBoard>&, int color);
};