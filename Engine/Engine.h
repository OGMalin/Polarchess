#pragma once

#include "EngineInterface.h"

class Engine
{
	friend void EngineSearchThreadLoop(void* eng);
public:
	Engine();
};