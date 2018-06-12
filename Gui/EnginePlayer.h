#pragma once

#include <QString>
#include <QMap>
#include <QVector>

class EnginePlayer
{
public:
	const QString name() { return _name; };
	const QString path() { return _path; };
	const QString book() { return _book; };
	int bookdepth() { return _bookdepth; };
	int elo() { return _elo; };
	const QMap<QString, QString> option() { return _option; };
private:
	QString _name;
	QString _path;
	QString _book;
	int _bookdepth;
	int _elo;
	QMap<QString, QString> _option;
	friend class EnginePlayers;
};

class EnginePlayers
{

public:
	EnginePlayers();
	EnginePlayers(const EnginePlayers& ep);
	EnginePlayers& operator=(const EnginePlayers& ep);
	QStringList getAll();
	EnginePlayer getEngine(QString& name);
private:
	QVector<EnginePlayer> engines;
	void read();
	void copy(const EnginePlayers& ep);
};