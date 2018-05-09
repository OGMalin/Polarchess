#pragma once

#include <Windows.h>
#include <list>
#include "ChessBoard.h"
#include "MoveList.h"

enum EngineCommand
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
	ENG_string			// in
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
	std::list<EngineCommand> inQueCmd;
	std::list<std::string> inQueStr;
	std::list<EngineCommand> outQueCmd;
	std::list<ChessBoard> outQueCb;
	std::list<EngineGo> outQueGo;
	EngineInterface();
	virtual ~EngineInterface();
	void sendInQue(EngineCommand cmd);
	void sendInQue(EngineCommand cmd, std::string& s);
	void sendOutQue(EngineCommand cmd, ChessBoard& cb);
	void sendOutQue(EngineCommand cmd);
	void sendOutQue(EngineCommand cmd, EngineGo& eg);
	EngineCommand peekInQue();
	EngineCommand getInQue();
	EngineCommand getInQue(std::string& s);
	EngineCommand peekOutQue();
	EngineCommand getOutQue();
	EngineCommand getOutQue(ChessBoard& cb);
	EngineCommand getOutQue(EngineGo& cb);
};
