#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QString>
#include <QVector>
#include <QByteArray>
#include <QDataStream>
#include "../Common/ChessMove.h"
#include "../Common/ChessBoard.h"
#include "Path.h"
#include "DatabaseFormat.h"

class Database : public QObject
{
	Q_OBJECT

public:
	BookDBInfo bdi;
	Database(QString name, QObject *parent=0);
	~Database();
	// dbtype: 0=general, 1=White repertoire, 2=Black repertoire
	bool create(const QString& path, int dbtype);
	bool open(const QString& path);
	void close();
	bool add(BookDBEntry& data);
	BookDBEntry find(ChessBoard&);
	bool isOpen() { return opened; };
	BookDBInfo bookInfo();
	void getTrainingPosition(QVector<BookDBEntry>&);
	void clearAllTrainingData();
	void importBase(Database* iBase);
	//void addTrainingLines(QVector<TrainingLine>& tlines);
	//void deleteTrainingLines();
	//bool getTrainingLines(QVector<TrainingLine>& lines);
	void updateTrainingScore(TrainingDBEntry&);

	// Get db path
	QString getPath();
private:
	bool opened;
//	QSqlDatabase bookdb;
	QString dbname;
	bool exist(ChessBoard& board);
};
