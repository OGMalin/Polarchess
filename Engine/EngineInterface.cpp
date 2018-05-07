#include <process.h>
#include "EngineInterface.h"

using namespace std;

CRITICAL_SECTION engineCS;
extern void EngineSearchThreadLoop(void* eng);

EngineInterface::EngineInterface()
{
	InitializeCriticalSection(&engineCS);
	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	hEngine = CreateEvent(NULL, FALSE, FALSE, NULL);
	hThread = (HANDLE)_beginthread(EngineSearchThreadLoop, 0, this);
}

EngineInterface::~EngineInterface()
{
	DeleteCriticalSection(&engineCS);
	CloseHandle(hEvent);
	CloseHandle(hEngine);
}

void EngineInterface::sendOutQue(EngineCommand cmd, ChessBoard& cb)
{
	EngineMessage msg;
	msg.cmd = cmd;
	memcpy(msg.data, &cb, sizeof(cb));
	EnterCriticalSection(&engineCS);
	outQue.push(msg);
	LeaveCriticalSection(&engineCS);
	SetEvent(hEngine);
}

void EngineInterface::sendOutQue(EngineCommand cmd)
{
	EngineMessage msg;
	msg.cmd = cmd;
	EnterCriticalSection(&engineCS);
	outQue.push(msg);
	LeaveCriticalSection(&engineCS);
	SetEvent(hEngine);
}

void EngineInterface::sendOutQue(EngineCommand cmd, EngineGo& eg)
{
	EngineMessage msg;
	msg.cmd = cmd;
	memcpy(msg.data, &eg, sizeof(eg));
	EnterCriticalSection(&engineCS);
	outQue.push(msg);
	LeaveCriticalSection(&engineCS);
	SetEvent(hEngine);
}

EngineCommand EngineInterface::peekOutQue()
{
	EngineMessage msg;
	EnterCriticalSection(&engineCS);
	msg=outQue.front();
	LeaveCriticalSection(&engineCS);
	return msg.cmd;
}

EngineCommand EngineInterface::getOutQue()
{
	EngineMessage msg;
	EnterCriticalSection(&engineCS);
	msg = outQue.front();
	outQue.pop();
	LeaveCriticalSection(&engineCS);
	return msg.cmd;
}

EngineCommand EngineInterface::peekInQue()
{
	EngineMessage msg;
	EnterCriticalSection(&engineCS);
	msg = inQue.front();
	LeaveCriticalSection(&engineCS);
	return msg.cmd;
}