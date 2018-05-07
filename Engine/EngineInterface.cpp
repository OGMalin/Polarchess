#include "EngineInterface.h"

using namespace std;

CRITICAL_SECTION engineCS;

EngineInterface::EngineInterface()
{
	InitializeCriticalSection(&engineCS);
	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}

EngineInterface::~EngineInterface()
{
	DeleteCriticalSection(&engineCS);
	CloseHandle(hEvent);
}

void EngineInterface::send(EngineCommand cmd, ChessBoard& cb)
{
	EngineMessage msg;
	msg.cmd = cmd;
	if (sizeof(cb) > MAX_ENGINEDATA)
		return;
	memcpy(msg.data, &cb, sizeof(cb));
	EnterCriticalSection(&engineCS);
	outQue.push(msg);
	LeaveCriticalSection(&engineCS);
}

void EngineInterface::send(EngineCommand cmd)
{
	EngineMessage msg;
	msg.cmd = cmd;
	EnterCriticalSection(&engineCS);
	outQue.push(msg);
	LeaveCriticalSection(&engineCS);
}
