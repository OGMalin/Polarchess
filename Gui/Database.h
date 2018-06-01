#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QString>


class Database : public QObject
{
	Q_OBJECT

private:
	QSqlDatabase db;
public:
	Database(QObject *parent=0);
	~Database();
	bool create();
};
