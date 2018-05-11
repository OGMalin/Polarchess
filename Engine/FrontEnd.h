#pragma once

#include <string>
#include "UCI.h"
#include "EngineInterface.h"
#include "ChessBoard.h"

class FrontEnd
{
	DWORD movegenTest(int depth, bool init = true, int ply = 0);
public:
	int pawnValue;
	int knightValue;
	int bishopValue;
	int rookValue;
	int queenValue;
	DWORD maxElo;
	int contempt;
	ChessBoard currentBoard;
	UCI uci;
	EngineInterface engine;
	bool debug;
	FrontEnd();
	virtual ~FrontEnd();
	int run();
	int uciInput();
	void engineInput();
	void uciUci();
	void uciDebug(const std::string& s);
	void uciIsready();
	void uciSetoption(const std::string& s);
	void uciRegister(const std::string& s);
	void uciUcinewgame();
	void uciPosition(const std::string& s);
	void uciGo(const std::string& s);
	void uciStop();
	void uciPonderhit();
	void uciMovegen(const std::string& s);
	bool isMoveText(const std::string& input);
	void findMaxElo();
};
