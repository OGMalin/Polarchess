#pragma once
#include "Database.h"
#include "../Common/ChessBoard.h"
#include "../Common/ChessMove.h"
#include <QString>
#include <QVector>
#include <QWidget>

struct TrainingStat
{
	int moveerror;
	void clear() { moveerror = 0; };
};

struct TrainingPathEntry
{
	ChessMove move;
	int endscore;
	void clear() { move.clear(); endscore = 0; };
};

struct TrainingPath
{
	QVector<TrainingPathEntry> moves;
	int rowid;
	int endscore;
	int color;
	int start;
	int current;
	ChessBoard endposition;
	void clear() { moves.clear(); endscore = 0; color = 0; current = 0; rowid = 0; endposition.clear(); };
	friend bool operator<(const TrainingPath& t1, const TrainingPath& t2) { return t1.endscore < t2.endscore; };
	bool isCorrect(ChessMove& move);
	bool nextMove(ChessMove& move);
};

class Training
{
private:
	Database* Base[2];
	QVector<TrainingPath> list;
	void walkThrough(ChessBoard& cur, TrainingPath& path, int ply, QVector<BookDBEntry>& pos, int color);
	void convertMoves(const QString& smoves, TrainingPath& tp);
public:
	Training();
	~Training();
	// Set the database to use (color=WHITE or BLACK)
	void SetDatabase(int color, Database* base);

	// Create traininglines from a position (cb) and database (WHITE or BLACK, -1=both)
	void create(QWidget* parent, ChessBoard& cb, int color=-1);

	// Get the most needed trainingline (With most error).
	bool get(TrainingPath& line);

	// Get all traininglines.
	void getAll(QVector<TrainingPath>& allTP);

	// Update endscore
	void updateScore(int color, ChessBoard& cb, int rowid, int score);
};