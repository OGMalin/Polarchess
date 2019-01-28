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
	int score=0;
	int whiterep=0;
	int blackrep=0;
	BookDBMove() { clear(); };
	void clear() { move.clear(); comment.clear(); score = 0; whiterep = 0; blackrep = 0; };
};

struct BookDBEntry
{
	ChessBoard board;
	QString opening;
	QString eco;
	int eval=0;
	QString computer;
	QString comment;
	int score=0;
	QVector<BookDBMove> movelist;
	bool dirty;
	BookDBEntry() { clear();  dirty = false; };
	void clear() { board.clear(); opening.clear(); eco.clear(); comment.clear(); movelist.clear(); computer.clear(); eval = 0; dirty = false; };
	bool moveExist(ChessMove& move);

	void convertToMoveList(QVector<BookDBMove>&, const QString&);
	void convertFromMoveList(const QVector<BookDBMove>&, QString&);
};

class Database : public QObject
{
	Q_OBJECT

public:
	Database(QString& name, QObject *parent=0);
	~Database();
	bool create(const QString& path);
	bool open(const QString& path);
	void close();
	bool add(BookDBEntry& data);
	BookDBEntry find(ChessBoard& board);
	bool isOpen() { return opened; };

private:
	bool opened;
//	QSqlDatabase bookdb;
	QString dbname;
	bool exist(ChessBoard& board);
};
