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
	void start();
	ULONGLONG read(int precision);
};
