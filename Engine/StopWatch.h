#pragma once

#include <Windows.h>

class StopWatch
{
	DWORD starttime;
public:
	void start();
	DWORD read();
};
