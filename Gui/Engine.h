#pragma once

#include "../Common/ChessBoard.h"
#include "../Common/MoveList.h"
#include "../Common/ChessMove.h"
#include "../Common/UciEngine.h"
#include <QObject>
#include <QString>
#include <QVector>

//#include <QProcess>
//#include <QStringList>
//

enum { BULLET = 0, BLITZ, RAPID, CLASSICAL };

struct EngineRating
{
	int category;
	int rating;
	int skill;
	QString personality;
};

class Engine :public QObject
{
	Q_OBJECT

private:
	UciEngine uci;
	QVector<EngineRating> rating;
public slots:
	void slotEngineStarted();
	void slotEngineStoped();
	void slotEngineMove(const QString&, const QString&);
//	void slotEngineInfo(const EngineInfo&);
signals:
	//void engineMessage(const QString&);
	void engineMove(const QString&, const QString&);
//	void engineInfo(const EngineInfo&);
	void engineStarted();
	void engineStoped();
public:
	EngineOption personality;
	EngineOption skill;
	EngineOption autoskill;
	EngineOption limitstrength;
	EngineOption elo;
	EngineOption ownbook;
//	EngineOption bookfile;
	QString name;
	QString author;
	ChessBoard currentBoard;
	bool readyok;
	QString waitCommand;
	Engine();
	virtual ~Engine();
//	// Load an engine
	bool load(QString& path);
//	// Unload an engine, this is called automatic when loading a new engine.
	void unload();
//	// Check to see if it has loaded an engine;
	bool isLoaded();
//	// Start analyzing a position, stop the analyze with stop()
//	void analyze(ChessBoard& board);
//	// Stop analyzing
//	void stop();
//	// Set multipv, not all engines support this.
//	void newGame();
//	void setMultiPV(int n);
//	// Some engines need a restart before searching on next position
//	bool needRestart();
//
	void search(ChessBoard& board, MoveList& moves, SEARCHTYPE searchtype, int wtime = 0, int winc = 0, int btime = 0, int binc = 0, int movestogo = 0);
//	void setEngine(QString& name, QString& dir) {};
//	bool loadSetup(QString& setup) { return false; };
	QString getPath();
	void init(QString& key, QString& val) { uci.init(key, val); };
	void clearInit() { uci.clearInit(); };
	int getRating(int skill, QString& personality, int category);
};