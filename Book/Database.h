#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QString>
#include <QVector>
#include <QByteArray>
#include <QDataStream>
#include "../Common/ChessMove.h"
#include "../Common/ChessBoard.h"

struct BookDBMove
{
	ChessMove move;
	QString comment;
	int score;
	int repertoire;
	void clear() { move.clear(); comment.clear(); score = 0; repertoire = 0; };
};

struct BookDBEntry
{
	ChessBoard board;
	QString opening;
	QString eco;
	QString comment;
	QVector<BookDBMove> movelist;
	bool dirty;
	BookDBEntry() { dirty = false; };
	void clear() { board.clear(); opening.clear(); eco.clear(); comment.clear(); movelist.clear(); dirty = false; };
	bool moveExist(ChessMove& move);
	void convertToMoveList(QVector<BookDBMove>&, const QString&);
	void convertFromMoveList(const QVector<BookDBMove>&, QString&);
};

class Database : public QObject
{
	Q_OBJECT

public:
	Database(QObject *parent=0);
	~Database();
	bool create(const QString& path);
	bool open(const QString& path);
	bool add(BookDBEntry& data);
	BookDBEntry find(ChessBoard& board);

private:
	bool opened;
	QSqlDatabase bookdb;

	bool exist(ChessBoard& board);
};
