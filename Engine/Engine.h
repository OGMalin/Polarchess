#pragma once

#include "EngineInterface.h"
#include "DrawTable.h"

class Engine
{
	friend void EngineSearchThreadLoop(void* eng);
public:
	bool debug;
	DrawTable drawTable;
	Engine();
};