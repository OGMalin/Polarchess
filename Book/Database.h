#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QString>
#include <QVector>

struct BookDBMove
{
	QString cmove;
	QString comment;
	int score;
};

struct BookDBEntry
{
	int id;
	QString fenmovelist;
	QString opening;
	QString variation;
	QString subvariation;
	QString eco;
	QString comment;
	QVector<BookDBMove> movelist;
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

private:
	QSqlDatabase bookdb;
};
