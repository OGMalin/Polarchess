#include "Engine.h"

Engine::Engine()
{
}

Engine::~Engine()
{

}

void Engine::setEngine(QString& name, QString& dir)
{
	engineName = name;
	workingDir = dir;
}

