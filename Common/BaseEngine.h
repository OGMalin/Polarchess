#pragma once

#include <QObject>
#include <QProcess>
#include <QString>
#include <QMap>
#include <string>

class BaseEngine :public QObject
{
	Q_OBJECT
protected:
	QProcess* process;
	QMap<QString, QString> initOptions;

	// Overload this to get the input from the engine
	virtual void fromEngine(std::string& input) {};
public slots:
	// From QProcess
	virtual void errorOccurred(QProcess::ProcessError error);
	virtual void finished(int exitCode, QProcess::ExitStatus exitStatus);
	virtual void readReadyStandardOutput();
	virtual void started();
signals:
	void finnishStartup();
	void engineMessage(const QString& msg);
public:
	BaseEngine(QObject*parent = 0);
	virtual bool load(QString& path);
	void init(QString&key, QString&val);
	void write(const char* sz);
	void write(QString& qs) { write(qs.toLatin1()); };
};