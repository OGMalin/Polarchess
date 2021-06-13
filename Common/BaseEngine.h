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

enum OPTION_TYPE {
	CHECK = 0,
	SPIN,
	COMBO,
	BUTTON,
	STRING,
	SAVE,
	RESET,
};

struct OptionCheck
{
	bool default;
	bool value;
	void clear() { default = value = false; };
};

struct OptionSpin
{
	int default;
	int min;
	int max;
	int value;
	void clear(){ default = min = max = value = 0; };
};

struct OptionCombo
{
	QString default;
	QStringList var;
	QString value;
	void clear() { default.clear(); var.clear(); value.clear(); };
};

struct OptionString
{
	QString default;
	QString value;
	void clear() { default.clear(); value.clear(); };
};

struct EngineOption
{
	QString name;
	OPTION_TYPE type;
	OptionCheck check;
	OptionSpin spin;
	OptionCombo combo;
	OptionString string;
	EngineOption() { clear(); };
	void clear()
	{
		name.clear();
		type = CHECK;
		check.clear();
		spin.clear();
		combo.clear();
		string.clear();
	}
};


struct EngineInfo
{
	int depth;
	int seldepth;
	unsigned long time;
	unsigned long nodes;
	MoveList pv;
	int multipv;
	int cp;
	int mate;
	bool lowerbound;
	bool upperbound;
	ChessMove currmove;
	int currmovenumber;
	int hashfull;
	unsigned long nps;
	unsigned long tbhits;
	unsigned long sbhits;
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
	QVector<EngineOption> engineOption;
	QProcess* process;
	QMap<QString, QString> initOptions;
	int searchtype;
	QString enginePath;
	QString _name;
	QString _author;
	// Overload this to get the input from the engine
	virtual void fromEngine(std::string& input) {};
public slots:
	// From QProcess
	virtual void slotReadyReadStandardOutput();
	virtual void slotStarted() {};
	virtual void slotFinished();
	virtual void slotFinishInit() {};
signals:
	void engineStarted();
	void engineMessage(const QString& msg);
	void engineInfo(const EngineInfo&);
	void engineMove(const QString& move, const QString& ponder);
	void engineStoped();
public:
	BaseEngine(QObject*parent = 0);
	virtual ~BaseEngine();
	virtual bool load(QString& path);
	virtual void unload();
	virtual bool isLoaded() { return (process == NULL) ? false : true; };
	void init(QString&key, QString&val);
	virtual void write(const char* sz) { write(QString(sz)); };
	virtual void write(QString& qs);
	virtual void analyze(ChessBoard& board) {};
	virtual void stop() {};
	virtual void setMultiPV(int n) {};
	virtual EngineOption getOption(QString& name);
	virtual const QString name() { return _name; };
	virtual const QString author() { return _author; };
	virtual const QString path() { return enginePath; };
};