#include "Database.h"
#include <QCoreApplication>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QDir>
#include <QVariant>
#include <QDebug>

const char* GAMEDBVERSION = "1.0";
const char* GAMEDBTYPE = "GAMEDB";

Database::Database(QObject *parent)
	: QObject(parent)
{
}

Database::~Database()
{
}

bool Database::addGame(QChessGame* game, bool savetopgn)
{
	QString qs;
	if (savetopgn)
	{
		QString path;
		game->getPgn(qs);
		if (!qs.isEmpty())
		{
			path = QStandardPaths::locate(QStandardPaths::DocumentsLocation, "/Polarchess", QStandardPaths::LocateDirectory);
			if (path.isEmpty())
			{
				QStringList l = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
				if (l.size())
				{
					path = l.front();
					path += "/Polarchess";
					QDir().mkdir(path);
				}
			}
		}
		QFile pgnfile = path + "/autosave.pgn";
		pgnfile.open(QIODevice::Append | QIODevice::Text);
		QTextStream out(&pgnfile);
		out << qs << "\n\n";
		pgnfile.close();
	}
	return true;
}