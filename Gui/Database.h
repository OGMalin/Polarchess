#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QString>
#include "../Common/QChessGame.h"

class Database : public QObject
{
	Q_OBJECT

private:
	QSqlDatabase gamedb;
	QSqlDatabase bookdb;
signals:
	void databaseMessage(const QString& msg);

public:
	Database(QObject *parent=0);
	~Database();
	bool create();
	bool addGame(QChessGame* game);
};
