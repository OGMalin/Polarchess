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

	hash			text
		position as hashkey.

	enginelist	text
		engine1|cp|depth|time|pv;engine2|cb|depth|time|pv
*/

struct TrainingDBInfo
{
	QString db;
	QString version;
};

struct TrainingStat
{
	int moveerror;
	void clear() { moveerror = 0; };
};

struct TrainingPathEntry
{
	ChessMove move;
	int score;
	void clear() { move.clear(); score = 0; };
};

struct TrainingPath
{
	QVector<TrainingPathEntry> moves;
	int rowid;
	int score;
	int color;
	int current;
	ChessBoard endposition;
	void clear() { moves.clear(); score = 0; color = 0; current = 0; rowid = 0; endposition.clear(); };
	friend bool operator<(const TrainingPath& t1, const TrainingPath& t2) { return t1.score < t2.score; };
	bool isCorrect(ChessMove& move);
	bool nextMove(ChessMove& move);
	ChessMove currentMove();
};

class Training
{
private:
	TrainingDBInfo tdi;
	Database* Base[2];
	QVector<TrainingPath> list;
	void walkThrough(ChessBoard& cur, TrainingPath& path, int ply, QVector<BookDBEntry>& pos, int color);
	void convertMoves(const QString& smoves, TrainingPath& tp);
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

	// Create traininglines.
	void createLines(QWidget* parent);

	// Get next training line
	bool get(TrainingPath& line, int color, ChessBoard& cb);

	// Get all traininglines.
	void getAll(QVector<TrainingPath>& allTP, int color=-1);

	// Update training score
	void updateScore(int color, ChessBoard& cb, int rowid, int score);

	// Get db path
	QString getPath();
};