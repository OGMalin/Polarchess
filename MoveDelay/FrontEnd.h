#pragma once

// Main program loop

#include "GUI.h"
#include "Engine.h"
#include "Search.h"
#include "..\Common\ChessBoard.h"
#include "..\Common\StopWatch.h"
#include "..\Common\PolyglotBook.h"

class FrontEnd
{
	int delay;
	bool bookmove;
	bool stoping;
	int wtime, btime;
	Search search;
	ChessBoard currentBoard;
	StopWatch watch;
	bool guiInput();
	void engineInput();
	void searchInput();
public:
	PolyglotBook polyglot;
	GUI gui;
	Engine engine;
	bool BestMove;
	bool disableDelay; // For testing
	std::string enginepath;
	FrontEnd();
	~FrontEnd();
	int run();
};