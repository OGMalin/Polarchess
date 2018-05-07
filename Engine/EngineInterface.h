#pragma once
#include <Windows.h>
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
ENG_move,
ENG_pv
};

class EngineInterface
{
public:
	HANDLE hEvent;
	EngineInterface();
	virtual ~EngineInterface();
	void send(EngineCommand cmd, ChessBoard& cb);
	void send(EngineCommand cmd);
	
//	EngineCommand get();
};
