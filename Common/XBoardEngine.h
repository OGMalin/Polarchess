#pragma once

#include <QString>
#include <QStringList>

struct XBoardFeature
{
	bool ping;
	bool setboard;
	bool playother;
	bool san;
	bool usermove;
	bool time;
	bool draw;
	bool sigint;
	bool sigterm;
	bool reuse;
	bool analyze;
	QString myname;
	QStringList variants;
	bool colors;
	bool ics;
	bool name;
	bool pause;
	bool nps;
	bool debug;
	bool memory;
	bool smp;
	QStringList egt;
	QStringList option;
};

class XBoardEngine
{
private:
	XBoardFeature feature;
public:
	XBoardEngine();
	void clear();
	int readLine(QString& line);
};
