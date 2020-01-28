#pragma once
#include "Database.h"
#include "../Common/ChessBoard.h"
#include "../Common/ChessMove.h"
#include <QString>
#include <QVector>
#include <QWidget>
#include <QSqlDatabase>

/*
	Database format

	color			text
		The color for the player.

	score	text
		Calculated score from attempt and score for each move

	moves
		move1|attempt|score;move1|attempt|score
*/

struct TrainingDBInfo
{
	QString db;
	QString version;
};

//struct TrainingStat
//{
//	int moveerror;
//	void clear() { moveerror = 0; };
//};

struct TrainingDBMove
{
	ChessMove move;
	int attempt;
	int score;
	void clear() { move.clear(); score = 0; };
};

struct TrainingDBEntry
{
	QVector<TrainingDBMove> moves;
	int rowid;
	int score; // Score for this line made by calculating score and attempt for each move.
	int color;
	int current;
//	ChessBoard endposition;
	void clear() { moves.clear(); score = 0; color = 0; current = 0; rowid = 0;/* endposition.clear(); */};
	friend bool operator<(const TrainingDBEntry& t1, const TrainingDBEntry& t2) { return t1.score < t2.score; };
	bool isCorrect(ChessMove& move);
	bool nextMove(ChessMove& move);
	ChessMove currentMove();
	QString MovesToString();
	void MovesFromString(const QString&);
};

class Training
{
private:
	TrainingDBInfo tdi;
	Database* Base[2];
	QVector<TrainingDBEntry> list;
	QVector<TrainingDBEntry> currentList;
	void walkThrough(ChessBoard& cur, TrainingDBEntry& path, int ply, QVector<BookDBEntry>& pos, int color);
//	void convertMovesFromString(const QString& smoves, TrainingDBEntry& tp);
//	void convertMovesToString(QString& smoves, TrainingDBEntry& tp);
	bool opened;
public:
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

	// Get all traininglines.
	void getAll(QVector<TrainingDBEntry>& allTP, int color=-1);

	// Update training score
	void updateScore(int color, ChessBoard& cb, int rowid, int score);

	// Get db path
	QString getPath();
};