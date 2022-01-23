#pragma once
#include <windows.h>
#include <string>
#include <list>

enum {
	GUI_none = 0,
	GUI_unknown,
	GUI_eboard,
	GUI_connect,
	GUI_disconnect,
	GUI_debug,
	GUI_quit,
};

class GUI
{
public:
	HANDLE hRead, hWrite;
	HANDLE hEvent;
	HANDLE hThread;
	std::list<std::string> inQue;

	GUI();
	virtual ~GUI();
	static void threadLoop(void* lpv);
	bool input(const std::string& s);
	int get(std::string& s);
	int extract(std::string& s);

	void write(const std::string& s);
};