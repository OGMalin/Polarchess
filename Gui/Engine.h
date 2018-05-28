#pragma once

#include <QObject>
#include <QString>

class Engine :public QObject
{
	Q_OBJECT

private:
	QString engineName;
	QString workingDir;

public:
	Engine();
	virtual ~Engine();
	void setEngine(QString& name, QString& dir);
};