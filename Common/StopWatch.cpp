#include "StopWatch.h"

void StopWatch::start()
{
	starttime = GetTickCount();
}

DWORD StopWatch::read()
{
	DWORD r = GetTickCount();
	if (r < starttime)
		return (r - (0xffffffff - starttime));
	return (r - starttime);
}