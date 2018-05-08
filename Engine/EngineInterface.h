#pragma once

#include <Windows.h>
#include <list>
#include "ChessBoard.h"

enum EngineCommand
{
	ENG_none=0,
	ENG_position,
	ENG_history,
	ENG_clearhistory,
	ENG_clearhash,
	ENG_go,
	ENG_stop,
	ENG_quit,
	ENG_ponder,
	ENG_ponderhit,
	ENG_move,
	ENG_debug,
	ENG_nodebug,
	ENG_pv
};

#define MAX_ENGINEDATA 1024

struct EngineMessage
{
	EngineCommand cmd;
	BYTE data[MAX_ENGINEDATA];
};

struct EngineGo
{
	DWORD minTime;
	DWORD maxTime;
	DWORD depth;
	DWORD nodes;
	DWORD mate;
};

class EngineInterface
{
public:
	HANDLE hEvent;
	HANDLE hEngine;
	HANDLE hThread;
	std::list<EngineMessage> inQue;
	std::list<EngineMessage> outQue;

	EngineInterface();
	virtual ~EngineInterface();
	void sendOutQue(EngineCommand cmd, ChessBoard& cb);
	void sendOutQue(EngineCommand cmd);
	void sendOutQue(EngineCommand cmd, EngineGo& eg);
	EngineCommand peekOutQue();
	EngineCommand peekInQue();
	EngineCommand getOutQue();
};
