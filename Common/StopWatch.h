#pragma once

#include <Windows.h>

namespace WatchPrecision {
	enum { Millisecond, Microsecond, Second };
};

class StopWatch
{
	bool usePrecision;
	ULONGLONG starttime;
	ULONGLONG frequency;
public:
	StopWatch() { usePrecision = false; starttime = 0; frequency = 0; };
	void start();
	ULONGLONG read(int precision);
};
