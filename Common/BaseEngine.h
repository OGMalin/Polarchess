#pragma once

#include <QObject>
#include <QProcess>
#include <QString>
#include <QMap>
#include <string>
#include "../Common/ChessBoard.h"
#include "../Common/MoveList.h"

enum SEARCHTYPE
{
	NO_SEARCH,
	NORMAL_SEARCH,
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

class BaseEngine :public QObject
{
	Q_OBJECT
protected:
	QProcess* process;
	QMap<QString, QString> initOptions;
	int searchtype;
	
	// Overload this to get the input from the engine
	virtual void fromEngine(std::string& input) {};
public slots:
	// From QProcess
	virtual void slotErrorOccurred(QProcess::ProcessError error);
	virtual void slotFinished(int exitCode, QProcess::ExitStatus exitStatus);
	virtual void slotReadyReadStandardOutput();
	virtual void slotStarted();
	virtual void slotFinnishInit() {};
signals:
	void finnishStartup();
	void engineMessage(const QString& msg);
	void engineInfo(const EngineInfo&);
	void engineMove(const QString& move, const QString& ponder);
	void engineStoped();
public:
	BaseEngine(QObject*parent = 0);
	virtual ~BaseEngine();
	virtual bool load(QString& path);
	void init(QString&key, QString&val);
	void write(const char* sz);
	void write(QString& qs) { write(qs.toLatin1()); };
	virtual void analyze(ChessBoard& board, MoveList& moves) {};
	virtual void stop() {};

};