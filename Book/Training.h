#pragma once
#include "Database.h"
#include "../Common/ChessBoard.h"
#include "../Common/ChessMove.h"
#include <QString>
#include <QVector>

struct TrainingPathEntry
{
	ChessMove move;
	int movescore;
	int endscore;
	void clear() { move.clear(); endscore = 0; };
};

struct TrainingPath
{
	QVector<TrainingPathEntry> moves;
	int endscore;
	int color;
	void clear() { moves.clear(); endscore = 0; color = 0; };
	friend bool operator<(const TrainingPath& t1, const TrainingPath& t2) { return t1.endscore < t2.endscore; };
};

class Training
{
private:
	Database* Base[2];
	QVector<TrainingPath> list;
	void walkThrough(ChessBoard& cur, TrainingPath& path, int ply, QVector<BookDBEntry>& pos);
public:
	Training();
	~Training();
	void SetDatabase(int color, Database* base);
	void create(ChessBoard& cb, int color=-1);
	bool get(TrainingLine& line);
};