#pragma once

// Interface for the real uci engine

#include <Windows.h>
#include <string>
#include <list>

enum {
	ENGINE_none = 0,
	ENGINE_other,
	ENGINE_uciok,
	ENGINE_bestmove,
	ENGINE_info
};

class Engine
{
public:
	std::list<std::string> inQue;
	bool abort;
	PROCESS_INFORMATION pi;
	HANDLE hRead, hWrite;
	HANDLE hEvent;
	HANDLE hThread;
	Engine();
	~Engine();
	static void engineInputThread(void* lpv);
	void input(const std::string& s);
	bool start(const std::string& s);
	void stop();
	void write(const std::string& s);
	int get(std::string& s);
	int extract(std::string& s);
};
