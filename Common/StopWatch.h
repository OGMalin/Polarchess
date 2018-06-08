#pragma once

#include <Windows.h>

namespace WatchPrecicion {
	enum { Millisecond, Microsecond, Second };
};

class StopWatch
{
	bool usePrecision;
	ULONGLONG starttime;
	ULONGLONG frequency;
public:
	StopWatch();
	void start();
	ULONGLONG read(int type);
};
