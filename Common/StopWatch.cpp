#include "StopWatch.h"


StopWatch::StopWatch()
{
}

void StopWatch::start()
{
	LARGE_INTEGER f;
	if (QueryPerformanceFrequency(&f))
	{
		LARGE_INTEGER li;
		usePrecision = true;
		QueryPerformanceCounter(&li);
		starttime = li.QuadPart;
		frequency = f.QuadPart;
	}
	else
	{
		usePrecision = false;
		starttime = GetTickCount();
	}
}

ULONGLONG StopWatch::read(int type)
{
	ULONGLONG r;
	if (usePrecision)
	{
		LARGE_INTEGER endtime,elapsed;
		QueryPerformanceCounter(&endtime);
		elapsed.QuadPart = endtime.QuadPart - starttime;
		elapsed.QuadPart *= 1000000;
		r = elapsed.QuadPart/frequency;
	}
	else
	{
		r = GetTickCount64();
		r -= starttime;
	}
	if (type == WatchPrecicion::Second)
		r /= 1000000;
	else if (type == WatchPrecicion::Millisecond)
		r /= 1000;
	return r;
}