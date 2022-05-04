#pragma once

// Main program loop

#include "GUI.h"
#include "Engine.h"
#include "Search.h"
#include "..\Common\ChessBoard.h"
#include "..\Common\StopWatch.h"
#include <map>

class FrontEnd
{
	std::map<std::string, std::string> transFromEngine;
	std::map<std::string, std::string> transFromGui;
	int delay;
	bool bookmove;
	bool stoping;
	int wtime, btime;
	GUI gui;
	Engine engine;
	Search search;
	ChessBoard currentBoard;
	StopWatch watch;
	bool guiInput();
	void engineInput();
	void searchInput();
public:
	std::string enginepath;
	FrontEnd();
	~FrontEnd();
	int run();
	void transEngine(std::string, char*);
	void transGui(std::string, char*);
};