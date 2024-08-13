#pragma once

// Interface for the GUI

#include <Windows.h>
#include <string>
#include <list>
#include <map>

enum {
	GUI_none = 0,
	GUI_other,
	GUI_setoption,
	GUI_go,
	GUI_position,
	GUI_stop,
	GUI_test,
	GUI_quit
};

class GUI
{
public:
	HANDLE hRead, hWrite;
	HANDLE hEvent;
	HANDLE hThread;
	std::list<std::string> inQue;
	std::map<std::string, std::string> translation;

	GUI();
	~GUI();
	static void guiInputThread(void* lpv);
	bool input(const std::string& s);
	int get(std::string& s);
	int extract(std::string& s);
	void write(const std::string& s);
	void readTranslation(std::string, char*);
	void translate(std::string&);
};

