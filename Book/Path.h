#pragma once

#include <QVector>
#include <QStringList>
#include "../Common/ChessMove.h"
#include "../Common/ChessBoard.h"

//extern const char* STARTFEN;

struct PathEntry
{
	ChessBoard board;
	ChessMove move;
};

class Path
{
private:
	// The path
	QVector<PathEntry> moves;

	// Pont to active position
	int _current;

	// Return
	int size();

	// Who is to move at current position
	typeColor toMove();

public:
	Path();
	virtual ~Path();

	// Add a move from current position to the path. If it exist as 'shadow' move the rest of the path are kept.
	bool add(ChessMove& move);

	// Clear the path and set the startposition to standard startposition.
	void clear();

	// Set current index;
	void current(int n);

	// Get current index;
	int current();

	// Return current position
	ChessBoard getPosition();

	// Get entry at index n.
	PathEntry getEntry(int n);

	// Return the startposition.
	ChessBoard getStartPosition();

	// Get full movelist.
	void getMoveList(QStringList& ml, const char* charset=NULL);
};