#include "EnginePlayer.h"
#include <QSettings>

EnginePlayers::EnginePlayers()
{
	read();
}

EnginePlayers::EnginePlayers(const EnginePlayers& ep)
{
	copy(ep);
}

EnginePlayers& EnginePlayers::operator=(const EnginePlayers& ep)
{
	copy(ep);
	return *this;
}

void EnginePlayers::copy(const EnginePlayers& ep)
{
	engines = ep.engines;
}

void EnginePlayers::read()
{
	QSettings settings("Engine.ini", QSettings::IniFormat);
	QStringList eng = settings.childGroups();
	QStringList::iterator eit = eng.begin();
	EnginePlayer p;
	while (eit != eng.end())
	{
		p._name = *eit;
		settings.beginGroup(*eit);
		p._path = settings.value("path", QString()).toString();
		p._book = settings.value("book", QString()).toString();
		p._bookdepth = settings.value("bookdepth", 999).toInt();
		p._elo = settings.value("elo", 0).toInt();
		settings.beginReadArray("option");
		QStringList opt = settings.childKeys();
		QStringList::iterator oit = opt.begin();
		while (oit != opt.end())
		{
			p._option[*oit] = settings.value(*oit, QString()).toString();
			++oit;
		}
		engines.push_back(p);
		settings.endArray();
		settings.endGroup();
		++eit;
	}
}

QStringList EnginePlayers::getAll()
{
	QStringList list;
	QVector<EnginePlayer>::iterator it=engines.begin();
	while (it != engines.end())
	{
		list.append(it->name());
		++it;
	}
	return list;
}

EnginePlayer EnginePlayers::getEngine(QString& name)
{
	QVector<EnginePlayer>::iterator it = engines.begin();
	while (it != engines.end())
	{
		if (it->name() == name)
			return *it;
		++it;
	}
	return EnginePlayer();
}
