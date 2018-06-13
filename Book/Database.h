#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QString>
#include <QVector>
#include <QByteArray>
#include <QDataStream>

struct BookDBMove
{
	QString cmove;
	QString comment;
	int score;
};

struct BookDBEntry
{
	QString fen;
	QString opening;
	QString variation;
	QString subvariation;
	QString eco;
	QString comment;
	QVector<BookDBMove> movelist;
	bool dirty;
	BookDBEntry() { dirty = false; };
	void clear() { fen.clear(); opening.clear(); variation.clear(); subvariation.clear(); eco.clear(); comment.clear(); movelist.clear(); dirty = false; };
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
	BookDBEntry find(QString& fen);

private:
	bool opened;
	QSqlDatabase bookdb;

	bool exist(QString& fen);
	QByteArray toDBFormat(const QVector<BookDBMove>& data);
	QVector<BookDBMove> fromDBFormat(const QByteArray& data);
};
