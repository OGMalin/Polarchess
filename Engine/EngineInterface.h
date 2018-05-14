#pragma once

#include <Windows.h>
#include <list>
#include "ChessBoard.h"
#include "MoveList.h"

enum ENGINECOMMAND
{
	ENG_none=0,			// in|out
	ENG_position,		// out
	ENG_history,		// out
	ENG_clearhistory,	// out
	ENG_clearhash,		// out
	ENG_go,				// out
	ENG_stop,			// out
	ENG_quit,			// out
	ENG_ponder,			// out
	ENG_ponderhit,		// out
	ENG_info,			// in
	ENG_debug,			// in|out
	ENG_nodebug,		// out
	ENG_eval,			// out
	ENG_string			// in
};

enum ENGINEEVAL
{
	EVAL_contempt=1,
	EVAL_multipv,
	EVAL_pawn,
	EVAL_knight,
	EVAL_bishop,
	EVAL_rook,
	EVAL_queen
};

struct EngineEval
{
	EngineEval() {};
	EngineEval(const ENGINEEVAL t, const int v) { type = t; value = v; };
	ENGINEEVAL type;
	int value;
};

struct EngineGo
{
	DWORD fixedTime;
	DWORD maxTime;
	DWORD depth;
	DWORD nodes;
	DWORD mate;
	MoveList searchmoves;
};

class EngineInterface
{
public:
	HANDLE hEvent;
	HANDLE hEngine;
	HANDLE hThread;
	std::list<ENGINECOMMAND> inQueCmd;
	std::list<std::string> inQueStr;
	std::list<ENGINECOMMAND> outQueCmd;
	std::list<ChessBoard> outQueCb;
	std::list<EngineGo> outQueGo;
	std::list<EngineEval> outQueEvl;
	EngineInterface();
	virtual ~EngineInterface();
	void sendInQue(ENGINECOMMAND cmd);
	void sendInQue(ENGINECOMMAND cmd, const std::string& s);
	void sendOutQue(ENGINECOMMAND cmd, const ChessBoard& cb);
	void sendOutQue(ENGINECOMMAND cmd);
	void sendOutQue(ENGINECOMMAND cmd, const EngineGo& eg);
	void sendOutQue(ENGINECOMMAND cmd, const EngineEval& e);
	ENGINECOMMAND peekInQue();
	ENGINECOMMAND getInQue();
	ENGINECOMMAND getInQue(std::string& s);
	ENGINECOMMAND peekOutQue();
	ENGINECOMMAND getOutQue();
	ENGINECOMMAND getOutQue(ChessBoard& cb);
	ENGINECOMMAND getOutQue(EngineGo& cb);
	ENGINECOMMAND getOutQue(EngineEval& e);
};
