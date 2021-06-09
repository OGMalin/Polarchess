#pragma once

#include <QString>
#include <QVector>
#include <QMap>
#include "../Common/ChessMove.h"
#include "../Common/ChessBoard.h"
#include "../Common/defs.h"

struct PolyglotDataEntry
{
	unsigned short move;
	unsigned short weight;
	unsigned int learn;
	PolyglotDataEntry() { move = weight = learn = 0; };
};

class PolyglotBook
{
public:
	QString filename;
	PolyglotBook();
	PolyglotBook(QString& bookfile);
	virtual ~PolyglotBook();
	bool open(QString& bookfile);
	void close();
	void save();
	void get(ChessBoard& board, QVector<PolyglotDataEntry>&);
	ChessMove move(ChessBoard&, unsigned short);
	HASHKEY getKey(ChessBoard&);
	void add(ChessBoard&, ChessMove&, unsigned short weight, unsigned int learn);
	bool exist(ChessBoard&);
	int moves();
	int positions();
private:
	bool isOpen;
	bool isDirty;
	QMap<HASHKEY, QVector<PolyglotDataEntry>> book;
	void add(HASHKEY, PolyglotDataEntry&);
};

