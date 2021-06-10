#pragma once
#include <QString>
#include <QObject>
#include <QStringList>

class QWidget;

class Player : public QObject
{
	Q_OBJECT
private:
	bool isDirty;
	QString sName;
	int nBulleElo;
	int nBulleGames;
	int nBlitzElo;
	int nBlitzGames;
	int nRapidElo;
	int nRapidGames;
	int nClassicalElo;
	int nClassicalGames;
public:
	Player(const QString playername=QString());
	~Player();
	const QString name() { return sName; };
	void name(QString& s) { if (!sName.isEmpty())save(); isDirty = true; sName = s; };
	const int bulletElo() { return nBulleElo; };
	void bulletElo(int n) { isDirty = true; nBulleElo = n; };
	const int bulleGames() { return nBulleGames; };
	void bulleGames(int n) { isDirty = true; nBulleGames = n; };
	const int blitzElo() { return nBlitzElo; };
	void blitzElo(int n) { isDirty = true; nBlitzElo = n; };
	const int blitzGames() { return nBlitzGames; };
	void blitzGames(int n) { isDirty = true; nBlitzGames = n; };
	const int rapidElo() { return nRapidElo; };
	void rapidElo(int n) { isDirty = true; nRapidElo = n; };
	const int rapidGames() { return nRapidGames; };
	void rapidGames(int n) { isDirty = true; nRapidGames = n; };
	const int classicalElo() { return nClassicalElo; };
	void classicalElo(int n) { isDirty = true; nClassicalElo = n; };
	const int classicalGames() { return nClassicalGames; };
	void classicalGames(int n) { isDirty = true; nClassicalGames = n; };
	bool save();
	bool load(const QString playername = QString());
	void clear();
	void newPlayer(QWidget* parent=0);
	const QStringList getAll();
	const void remove(const QString playername);
};