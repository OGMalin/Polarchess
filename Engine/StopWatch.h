#pragma once

#include <Windows.h>

class StopWatch
{
	DWORD starttime;
public:
	StopWatch() { starttime = 0; };
	void start();
	DWORD read();
};
