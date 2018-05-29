#pragma once

#include <QObject>
#include <QString>
#include <QProcess>

class Engine :public QObject
{
	Q_OBJECT

private:
	QString engineName;
	QString workingDir;
	QProcess* process;
	QString lasterror;
public slots:
	// From QProcess
	void slotErrorOccurred(QProcess::ProcessError error);
	void slotFinished(int exitCode, QProcess::ExitStatus exitStatus);
	void slotReadyStandardOutput();
	void slotStarted();
	void slotStateChanged(QProcess::ProcessState newState);
	// From IO Device
	void slotReadyRead();
signals:
	void engineMessage(const QString& msg);
public:
	Engine();
	virtual ~Engine();
	void setEngine(QString& name, QString& dir);
	bool load();
	void unload();
	const QString lastError();
};