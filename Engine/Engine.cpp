#include "Engine.h"
#include "EngineInterface.h"
#include <process.h>

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
		cmd = ei->getOutQue();
	}
	_endthread();
};


Engine::Engine()
{

}