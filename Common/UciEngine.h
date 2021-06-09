#pragma once

#include <QString>
#include <string>
#include "../Common/BaseEngine.h"

class UciEngine:public BaseEngine
{
private:
	ChessBoard currentBoard;
protected:
	virtual void fromEngine(std::string& input);
public slots:
	virtual void slotStarted();
	virtual void slotFinishInit();
public:
	UciEngine();
	virtual ~UciEngine();
	void analyze(ChessBoard& board);
	void stop();
	void newGame();
	void setMultiPV(int n);
	void unload();
};