#pragma once

#include "../Common/ChessBoard.h"
#include "../Common/MoveList.h"
#include "../Common/ChessMove.h"
#include <QObject>
#include <QString>
#include <QProcess>
#include <QStringList>

enum SEARCHTYPE
{
	NORMAL_SEARCH = 1,
	PONDER_SEARCH,
	NODES_SEARCH,
	MATE_SEARCH,
	DEPTH_SEARCH,
	TIME_SEARCH,
	INFINITE_SEARCH
};
struct EngineInfo
{
	int depth;
	int seldepth;
	int time;
	int nodes;
	MoveList pv;
	int multipv;
	int cp;
	int mate;
	bool lowerbound;
	bool upperbound;
	ChessMove currmove;
	int currmovenumber;
	int hashfull;
	int nps;
	int tbhits;
	int sbhits;
	int cpuload;
	QString string;
	MoveList refutation;
	int cpunr;
	MoveList currline;
	EngineInfo()
	{
		depth = 0;
		seldepth = 0;
		time = 0;
		nodes = 0;
		multipv = 0;
		cp = 0;
		mate = 0;
		lowerbound = false;
		upperbound = false;
		currmovenumber = 0;
		hashfull = 0;
		nps = 0;
		tbhits = 0;
		sbhits = 0;
		cpuload = 0;
		cpunr = 0;
	};
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
	QStringList options;
	ChessBoard currentBoard;
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
	void engineInfo(const EngineInfo&);
public:
	Engine();
	virtual ~Engine();
	void setEngine(QString& name, QString& dir);
	bool loadSetup(QString& setup);
	bool load(QString& enginefile);
	void unload();
	void write(QString& cmd);
	void write(char* sz) { write(QString(sz)); }
	void search(ChessBoard& board, MoveList& moves, SEARCHTYPE searchtype, int wtime = 0, int winc = 0, int btime = 0, int binc = 0, int movestogo = 0);
	const QString lastError();
	void stop();
	void setMultiPV(int n);
};