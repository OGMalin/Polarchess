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
	QVector<TrainingLine> currentLines;
	TrainingStatistics stat;
	ChessBoard startBoard;
	int startColor;
	void walkThrough(ChessBoard& cur, TrainingLine& line, int ply, QVector<BookDBEntry>& pos, int color);
	//	void convertMovesFromString(const QString& smoves, TrainingDBEntry& tp);
//	void convertMovesToString(QString& smoves, TrainingDBEntry& tp);
	bool equalLine(TrainingLine& tl1, TrainingLine& tl2);
public:
	Database* Base[2];

	Training();
	~Training();

	// Set the database to use (color=WHITE or BLACK)
	void SetRepertoireDatabase(int color, Database* base);

	// Removing all trainingdata and statistics
	void clearAll();


	// Get next trainingline
	bool getNextLine(TrainingLine& line);
	bool createLines(QWidget* parent, int color, ChessBoard& cb);

	// Update training score
	void updateScore(TrainingLine&);

	TrainingStatistics getStat();

	void getEndpositions(QVector<ChessBoard>&, int color);
};