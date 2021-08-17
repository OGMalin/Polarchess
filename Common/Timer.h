#pragma once

class Timer
{
public:
	Timer();
	~Timer();
	void start(int ms);
	void stop();
};