#pragma once

#include <QString>
#include <QVector>
#include "../Common/ChessMove.h"
#include "../Common/ChessBoard.h"

struct PolyglotEntry
{
	unsigned __int64 key;
	unsigned __int16 move;
	unsigned __int16 weight;
	unsigned __int32 learn;
};

class Polyglot
{
public:
	Polyglot();
	Polyglot(QString& bookfile);
	virtual ~Polyglot();
	bool open(QString& bookfile);
	void close();
	void get(ChessBoard& board, QVector<ChessMove>& moves);
private:
	bool isOpen;
};

