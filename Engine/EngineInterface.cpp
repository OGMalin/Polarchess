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

}

void EngineInterface::send(EngineCommand cmd)
{

}
