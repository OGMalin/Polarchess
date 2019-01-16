#pragma once

#include <QVector>
#include <QStringList>
#include "../Common/ChessMove.h"
#include "../Common/ChessBoard.h"

extern const char* STARTFEN;

struct PathEntry
{
	ChessBoard board;
	ChessMove move;
};

class Path
{
private:
	QVector<PathEntry> moves;
public:
	Path();
	virtual ~Path();
	ChessBoard getStartPosition();
	ChessBoard getPosition();
	void clear();
	typeColor toMove();
	bool add(ChessMove& move);
	int size();
	void setLength(int ply);
	PathEntry getEntry(int n);
	void getMoveList(QStringList& ml);
};