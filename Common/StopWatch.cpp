#include "StopWatch.h"

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
		starttime = GetTickCount64();
	}
}

ULONGLONG StopWatch::read(int precision)
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
	if (precision == WatchPrecision::Second)
		r /= 1000000;
	else if (precision == WatchPrecision::Millisecond)
		r /= 1000;
	return r;
}