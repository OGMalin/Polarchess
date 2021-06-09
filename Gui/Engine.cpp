#include "Engine.h"
#include <QApplication>

Engine::Engine()
{
}

Engine::~Engine()
{
	unload();
}

bool Engine::load(QString& path)
{
	QMetaObject::Connection con;


	con = connect(&uci, SIGNAL(engineStarted()), SLOT(slotEngineStarted()));
	con = connect(&uci, SIGNAL(engineStoped()), SLOT(slotEngineStoped()));
//	con = connect(&uci, SIGNAL(engineInfo(const EngineInfo&)), SLOT(slotEngineInfo(const EngineInfo&)));
	if (!uci.load(path))
		return false;
	return true;
}

void Engine::unload()
{
	uci.disconnect();
	uci.unload();
}

void Engine::search(ChessBoard& board, MoveList& moves, SEARCHTYPE searchtype, int wtime, int winc, int btime, int binc, int movestogo)
{
}

void Engine::slotEngineStarted()
{
	int i;
	personality = uci.getOption(QString("Personality"));
	skill = uci.getOption(QString("Skill"));
	autoskill = uci.getOption(QString("Auto Skill"));
	limitstrength = uci.getOption(QString("UCI_LimitStrength"));
	elo = uci.getOption(QString("UCI_Elo"));

	name = uci.name();
	author = uci.author();
	emit engineStarted();
}

void Engine::slotEngineStoped()
{
	uci.disconnect();
	emit engineStoped();
}

//void Engine::slotEngineInfo(const EngineInfo& ei)
//{
//	emit engineInfo(ei);
//}

QString Engine::getPath()
{
	return uci.path();
}
