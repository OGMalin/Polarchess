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

void EngineInterface::sendInQue(EngineCommand cmd)
{
	EnterCriticalSection(&engineCS);
	inQueCmd.push_back(cmd);
	LeaveCriticalSection(&engineCS);
	SetEvent(hEvent);
}

void EngineInterface::sendInQue(EngineCommand cmd, std::string& s)
{
	EnterCriticalSection(&engineCS);
	inQueCmd.push_back(cmd);
	inQueStr.push_back(s);
	LeaveCriticalSection(&engineCS);
	SetEvent(hEvent);
}

void EngineInterface::sendOutQue(EngineCommand cmd)
{
	EnterCriticalSection(&engineCS);
	if (cmd == ENG_quit)
		outQueCmd.clear();
	outQueCmd.push_back(cmd);
	LeaveCriticalSection(&engineCS);
	SetEvent(hEngine);
}

void EngineInterface::sendOutQue(EngineCommand cmd, ChessBoard& cb)
{
	EnterCriticalSection(&engineCS);
	outQueCmd.push_back(cmd);
	outQueCb.push_back(cb);
	LeaveCriticalSection(&engineCS);
	SetEvent(hEngine);
}

void EngineInterface::sendOutQue(EngineCommand cmd, EngineGo& eg)
{
	EnterCriticalSection(&engineCS);
	outQueCmd.push_back(cmd);
	outQueGo.push_back(eg);
	LeaveCriticalSection(&engineCS);
	SetEvent(hEngine);
}

EngineCommand EngineInterface::peekOutQue()
{
	EngineCommand cmd;
	EnterCriticalSection(&engineCS);
	if (outQueCmd.size() > 0)
		cmd = outQueCmd.front();
	else
		cmd = ENG_none;
	LeaveCriticalSection(&engineCS);
	return cmd;
}

EngineCommand EngineInterface::getOutQue()
{
	EngineCommand cmd;
	EnterCriticalSection(&engineCS);
	if (outQueCmd.size() > 0)
	{
		cmd = outQueCmd.front();
		outQueCmd.pop_front();
	}
	else
	{
		cmd = ENG_none;
	}
	LeaveCriticalSection(&engineCS);
	return cmd;
}

EngineCommand EngineInterface::getOutQue(ChessBoard& cb)
{
	EngineCommand cmd;
	EnterCriticalSection(&engineCS);
	if (outQueCmd.size() > 0)
	{
		cmd = outQueCmd.front();
		outQueCmd.pop_front();
		if (outQueCb.size() > 0)
		{
			cb = outQueCb.front();
			outQueCb.pop_front();
		}
	}
	else
	{
		cmd = ENG_none;
	}
	LeaveCriticalSection(&engineCS);
	return cmd;
}

EngineCommand EngineInterface::getOutQue(EngineGo& eg)
{
	EngineCommand cmd;
	EnterCriticalSection(&engineCS);
	if (outQueCmd.size() > 0)
	{
		cmd = outQueCmd.front();
		outQueCmd.pop_front();
		if (outQueGo.size() > 0)
		{
			eg = outQueGo.front();
			outQueGo.pop_front();
		}
	}
	else
	{
		cmd = ENG_none;
	}
	LeaveCriticalSection(&engineCS);
	return cmd;
}

EngineCommand EngineInterface::peekInQue()
{
	EngineCommand cmd;
	EnterCriticalSection(&engineCS);
	if (inQueCmd.size() > 0)
		cmd = inQueCmd.front();
	else
		cmd = ENG_none;
	LeaveCriticalSection(&engineCS);
	return cmd;
}

EngineCommand EngineInterface::getInQue()
{
	EngineCommand cmd;
	EnterCriticalSection(&engineCS);
	if (inQueCmd.size() > 0)
	{
		cmd = inQueCmd.front();
		inQueCmd.pop_front();
	}
	else
	{
		cmd = ENG_none;
	}
	LeaveCriticalSection(&engineCS);
	return cmd;
}

EngineCommand EngineInterface::getInQue(std::string& s)
{
	EngineCommand cmd;
	EnterCriticalSection(&engineCS);
	if (inQueCmd.size() > 0)
	{
		cmd = inQueCmd.front();
		inQueCmd.pop_front();
		if (inQueStr.size() > 0)
		{
			s = inQueStr.front();
			inQueStr.pop_front();
		}
	}
	else
	{
		cmd = ENG_none;
	}
	LeaveCriticalSection(&engineCS);
	return cmd;
}

