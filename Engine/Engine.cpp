#include <process.h>
#include "Engine.h"
#include "EngineInterface.h"

void EngineSearchThreadLoop(void* lpv)
{
	EngineMessage msg;
	Engine eng;
	EngineCommand cmd;
	EngineInterface* ei = (EngineInterface*)lpv;

	while (1)
	{
		WaitForSingleObject(ei->hEngine, 5000);
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
		case ENG_stop: // No need to do anything here because the engine is in waiting state.
			cmd = ei->getOutQue();
			break;
		case ENG_clearhistory:
			cmd = ei->getOutQue();
			eng.drawTable.clear();
			break;
		case ENG_ponderhit:
		case ENG_clearhash:
		case ENG_go:
		case ENG_history:
		case ENG_ponder:
		case ENG_position:
		default:
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