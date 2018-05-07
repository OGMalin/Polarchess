#pragma once
#include <Windows.h>
#include <queue>
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
ENG_pv
};

#define MAX_ENGINEDATA 1024

struct EngineMessage
{
	int cmd;
	BYTE data[MAX_ENGINEDATA];
};

class EngineInterface
{
public:
	HANDLE hEvent;
	HANDLE hThread;
	std::queue<EngineMessage> inQue;
	std::queue<EngineMessage> outQue;

	EngineInterface();
	virtual ~EngineInterface();
	void send(EngineCommand cmd, ChessBoard& cb);
	void send(EngineCommand cmd);
	
//	EngineCommand get();
};
