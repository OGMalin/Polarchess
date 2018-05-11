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

void EngineInterface::sendInQue(ENGINECOMMAND cmd)
{
	EnterCriticalSection(&engineCS);
	inQueCmd.push_back(cmd);
	LeaveCriticalSection(&engineCS);
	SetEvent(hEvent);
}

void EngineInterface::sendInQue(ENGINECOMMAND cmd, const std::string& s)
{
	EnterCriticalSection(&engineCS);
	inQueCmd.push_back(cmd);
	inQueStr.push_back(s);
	LeaveCriticalSection(&engineCS);
	SetEvent(hEvent);
}

void EngineInterface::sendOutQue(ENGINECOMMAND cmd)
{
	EnterCriticalSection(&engineCS);
	if (cmd == ENG_quit)
		outQueCmd.clear();
	outQueCmd.push_back(cmd);
	LeaveCriticalSection(&engineCS);
	SetEvent(hEngine);
}

void EngineInterface::sendOutQue(ENGINECOMMAND cmd, const ChessBoard& cb)
{
	EnterCriticalSection(&engineCS);
	outQueCmd.push_back(cmd);
	outQueCb.push_back(cb);
	LeaveCriticalSection(&engineCS);
	SetEvent(hEngine);
}

void EngineInterface::sendOutQue(ENGINECOMMAND cmd, const EngineGo& eg)
{
	EnterCriticalSection(&engineCS);
	outQueCmd.push_back(cmd);
	outQueGo.push_back(eg);
	LeaveCriticalSection(&engineCS);
	SetEvent(hEngine);
}

void EngineInterface::sendOutQue(ENGINECOMMAND cmd, const EngineEval& e)
{
	EnterCriticalSection(&engineCS);
	outQueCmd.push_back(cmd);
	outQueEvl.push_back(e);
	LeaveCriticalSection(&engineCS);
	SetEvent(hEngine);
}

ENGINECOMMAND EngineInterface::peekOutQue()
{
	ENGINECOMMAND cmd;
	EnterCriticalSection(&engineCS);
	if (outQueCmd.size() > 0)
		cmd = outQueCmd.front();
	else
		cmd = ENG_none;
	LeaveCriticalSection(&engineCS);
	return cmd;
}

ENGINECOMMAND EngineInterface::getOutQue()
{
	ENGINECOMMAND cmd;
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

ENGINECOMMAND EngineInterface::getOutQue(ChessBoard& cb)
{
	ENGINECOMMAND cmd;
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

ENGINECOMMAND EngineInterface::getOutQue(EngineGo& eg)
{
	ENGINECOMMAND cmd;
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

ENGINECOMMAND EngineInterface::getOutQue(EngineEval& e)
{
	ENGINECOMMAND cmd;
	EnterCriticalSection(&engineCS);
	if (outQueCmd.size() > 0)
	{
		cmd = outQueCmd.front();
		outQueCmd.pop_front();
		if (outQueEvl.size() > 0)
		{
			e = outQueEvl.front();
			outQueEvl.pop_front();
		}
	}
	else
	{
		cmd = ENG_none;
	}
	LeaveCriticalSection(&engineCS);
	return cmd;
}

ENGINECOMMAND EngineInterface::peekInQue()
{
	ENGINECOMMAND cmd;
	EnterCriticalSection(&engineCS);
	if (inQueCmd.size() > 0)
		cmd = inQueCmd.front();
	else
		cmd = ENG_none;
	LeaveCriticalSection(&engineCS);
	return cmd;
}

ENGINECOMMAND EngineInterface::getInQue()
{
	ENGINECOMMAND cmd;
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

ENGINECOMMAND EngineInterface::getInQue(std::string& s)
{
	ENGINECOMMAND cmd;
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

