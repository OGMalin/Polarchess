#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QString>

class Database : public QObject
{
	Q_OBJECT

public:
	Database(QObject *parent=0);
	~Database();
	bool create(const QString& path);
	bool open(const QString& path);

private:
	QSqlDatabase bookdb;
};
