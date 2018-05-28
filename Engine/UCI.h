#pragma once

#include <windows.h>
#include <string>
#include <list>

enum {
	UCI_none = 0,
	UCI_unknown,
	UCI_movegen,
	UCI_debug,
	UCI_go,
	UCI_isready,
	UCI_ponderhit,
	UCI_position,
	UCI_quit,
	UCI_register,
	UCI_setoption,
	UCI_stop,
	UCI_uci,
	UCI_ucinewgame,
	UCI_eval,
	UCI_readfile
};

class Uci
{
public:
	HANDLE hRead,hWrite;
	HANDLE hEvent;
	HANDLE hThread;
	std::list<std::string> inQue;

	static void threadLoop(void* lpv);
	bool input(const std::string& s);
	Uci();
	virtual ~Uci();
	int get(std::string& s);
	int extract(std::string& s);

	void write(const std::string& s);
};

