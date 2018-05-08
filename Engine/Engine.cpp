#include <process.h>
#include "Engine.h"
#include "EngineInterface.h"

void EngineSearchThreadLoop(void* lpv)
{
	Engine eng;
	EngineCommand cmd;
	EngineInterface* ei = (EngineInterface*)lpv;
	ChessBoard cb;
	EngineGo eg;

	while (1)
	{
		WaitForSingleObject(ei->hEngine, INFINITE);
		cmd=ei->peekOutQue();
		if (cmd == ENG_quit)
			break;
		switch (cmd)
		{
		case ENG_debug:
			eng.debug = true;
			cmd = ei->getOutQue();
			break;
		case ENG_nodebug:
			eng.debug = false;
			cmd = ei->getOutQue();
			break;
		case ENG_stop: // No need to do anything here because the engine is in waiting state (or should it sent bestmove 0000 ?).
			cmd = ei->getOutQue();
			break;
		case ENG_clearhistory:
			cmd = ei->getOutQue();
			eng.drawTable.clear();
			break;
		case ENG_history:
			cmd = ei->getOutQue(cb);
			eng.drawTable.add(cb);
			break;
		case ENG_ponderhit:
			cmd = ei->getOutQue();
			break;
		case ENG_clearhash:
			cmd = ei->getOutQue();
			break;
		case ENG_go:
			cmd = ei->getOutQue(eg);
			break;
		case ENG_ponder:
			cmd = ei->getOutQue(eg);
			break;
		case ENG_position:
			cmd = ei->getOutQue(cb);
			break;
		default:
			// Unknown command, remove it.
			cmd = ei->getOutQue();
			break;
		}
	}
	_endthread();
};


Engine::Engine()
{
	debug = false;
}