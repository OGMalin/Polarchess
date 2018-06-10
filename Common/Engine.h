#pragma once

#include "../Common/QChessGame.h"
#include <QObject>
#include <QString>
#include <QProcess>

enum SEARCHTYPE
{
	NORMAL_SEARCH = 1,
	PONDER_SEARCH,
	NODES_SEARCH,
	MATE_SEARCH,
	DEPTH_SEARCH,
	TIME_SEARCH
};

class Engine :public QObject
{
	Q_OBJECT

private:
	QString engineName;
	QString workingDir;
	QProcess* process;
	QString lasterror;
	QString setup;
	bool readyok;
	QString waitCommand;
private slots:
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
	void engineReady();
	void engineMove(const QString& move, const QString& ponder);
public:
	Engine();
	virtual ~Engine();
	void setEngine(QString& name, QString& dir);
	bool load(QString& setup);
	void unload();
	void write(QString& cmd);
	void search(QChessGame* game, SEARCHTYPE searchtype, int wtime, int winc, int btime, int binc, int movestogo);
	const QString lastError();
};