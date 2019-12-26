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
	UciEngine* uci;
	XBoardEngine* xboard;
public slots:
	void slotEngineStarted();
	void slotEngineStoped();
	void slotEngineInfo(const EngineInfo&);
signals:
	void engineMessage(const QString&);
	void engineMove(const QString&, const QString&);
	void engineInfo(const EngineInfo&);
	void engineStarted();
	void engineStoped();
public:
	Engine();
	virtual ~Engine();
	// Load an engine
	bool load(QString& enginefile);
	// Unload an engine, this is called automatic when loading a new engine.
	void unload();
	// Start analyzing a position, stop the analyze with stop()
	void analyze(ChessBoard& board);
	// Stop analyzing
	void stop();
	// Set multipv, not all engines support this.
	void setMultiPV(int n);

	void search(ChessBoard& board, MoveList& moves, SEARCHTYPE searchtype, int wtime = 0, int winc = 0, int btime = 0, int binc = 0, int movestogo = 0);
	void setEngine(QString& name, QString& dir) {};
	bool loadSetup(QString& setup) { return false; };
};