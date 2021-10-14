#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QString>
#include "../Common/QChessGame.h"

class Database : public QObject
{
	Q_OBJECT

public:
	Database(QObject *parent=0);
	~Database();
	bool addGame(QChessGame* game, bool savetopgn);
};
