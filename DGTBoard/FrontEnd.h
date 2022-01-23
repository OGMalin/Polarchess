#pragma once
#include "GUI.h"
#include "DGT.h"

class FrontEnd
{
	GUI gui;
	DGT dgt;
	bool debug;
public:
	FrontEnd();
	virtual ~FrontEnd();
	int run();
	bool guiInput();
};