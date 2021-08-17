#pragma once
#include <Windows.h>

class Timer
{
	bool active;
public:
//	template<typename Function>
	void start(auto function, int ms);
	void stop();
};