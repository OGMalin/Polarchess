#pragma once

#include "../Common/ChessBoard.h"
#include "../Common/MoveList.h"
#include "../Common/ChessMove.h"
#include "../Common/UciEngine.h"
#include "../Common/XBoardEngine.h"
#include <QObject>
#include <QString>
#include <QProcess>
#include <QStringList>

class Engine :public QObject
{
	Q_OBJECT

private:
	QString engineName;
	QString workingDir;
	UciEngine* uci;
	XBoardEngine* xboard;
	QString lasterror;
	QString setup;
	bool readyok;
	bool waitforever;
	QString waitCommand;
	ChessBoard currentBoard;
	bool startup;
public slots:
//	void ready() {};
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
	void analyze(ChessBoard& board, MoveList& moves);
	void search(ChessBoard& board, MoveList& moves, SEARCHTYPE searchtype, int wtime = 0, int winc = 0, int btime = 0, int binc = 0, int movestogo = 0);
	void stop();
	void setMultiPV(int n);
};