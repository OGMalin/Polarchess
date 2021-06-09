#include "../Common/Engine.h"
#include "../Common/Utility.h"
#include <string>
#include <QSettings>
#include <QDebug>
#include <QTimer>

using namespace std;

Engine::Engine()
{
	uci = NULL;
	xboard = NULL;
}

Engine::~Engine()
{
	unload();
}
/*
void Engine::setEngine(QString& name, QString& dir)
{
	unload();
	engineName = name;
	workingDir = dir;
	QString path = dir + "/" + name;
}
*/

bool Engine::load(QString& enginefile)
{
	QSettings settings(enginefile, QSettings::IniFormat);
	settings.beginGroup("Engine");
	QString enginepath = settings.value("path").toString();
	if (enginepath.isEmpty())
		return false;
	QString qs=settings.value("type").toString();
	if (QString::compare(qs, "uci", Qt::CaseInsensitive) == 0)
		uci = new UciEngine();
	else if (QString::compare(qs, "winboard", Qt::CaseInsensitive) == 0)
		xboard = new XBoardEngine();
	else if (QString::compare(qs, "xboard", Qt::CaseInsensitive) == 0)
		xboard = new XBoardEngine();
	else
		return false;
	settings.endGroup();
	settings.beginGroup("Option");
	QStringList keys = settings.allKeys();

	QString val;
	for (int i = 0; i < keys.size(); i++)
		uci ? uci->init(keys[i], settings.value(keys[i]).toString()):xboard->init(keys[i], settings.value(keys[i]).toString());
	settings.endGroup();

	QMetaObject::Connection con;

	if (uci)
	{
		con = connect(uci, SIGNAL(engineStarted()), SLOT(slotEngineStarted()));
		con = connect(uci, SIGNAL(engineStoped()), SLOT(slotEngineStoped()));
		con = connect(uci, SIGNAL(engineInfo(const EngineInfo&)), SLOT(slotEngineInfo(const EngineInfo&)));
		return uci->load(enginepath);
	}
	
	con = connect(xboard, SIGNAL(engineStarted()), SLOT(slotEngineStarted()));
	con = connect(xboard, SIGNAL(engineStoped()), SLOT(slotEngineStoped()));
	con = connect(xboard, SIGNAL(engineInfo(const EngineInfo&)), SLOT(slotEngineInfo(const EngineInfo&)));
	return xboard->load(enginepath);
}

void Engine::slotEngineStarted()
{
	emit engineStarted();
}

void Engine::slotEngineStoped()
{
	emit engineStoped();
}

void Engine::slotEngineInfo(const EngineInfo& ei)
{
	emit engineInfo(ei);
}
/*
bool Engine::loadSetup(QString& setup)
{
	readyok = false;
	if (process)
		return false;

	if (engineName.isEmpty())
		return false;
	
	process = new QProcess(this);

	if (!process)
		return false;

	connect(process, SIGNAL(errorOccurred(QProcess::ProcessError)),  SLOT(slotErrorOccurred(QProcess::ProcessError)));
	connect(process, SIGNAL(finished(int, QProcess::ExitStatus)),  SLOT(slotFinished(int, QProcess::ExitStatus)));
	connect(process, SIGNAL(readyReadStandardOutput()), SLOT(slotReadyStandardOutput()));
	connect(process, SIGNAL(started()), SLOT(slotStarted()));
	//	connect(process, SIGNAL(stateChanged(QProcess::ProcessState newState)), SLOT(slotStateChanged(QProcess::ProcessState newState)));
	//	connect(process, SIGNAL(readyRead()), SLOT(slotReadyRead()));

	process->setReadChannel(QProcess::StandardOutput);

	if (!workingDir.isEmpty())
		process->setWorkingDirectory(workingDir);

	emit engineMessage("Starting engine");
	process->start(engineName);
	
	return true;
}
*/
void Engine::unload()
{
	if (xboard)
	{
		disconnect(xboard);
		delete xboard;
		xboard = NULL;
	}
	else if (uci)
	{
		disconnect(uci);
		delete uci;
		uci = NULL;
	}
}

void Engine::analyze(ChessBoard& board)
{
	if (xboard)
		xboard->analyze(board);
	else if (uci)
		uci->analyze(board);
}

void Engine::search(ChessBoard& board, MoveList& moves, SEARCHTYPE searchtype, int wtime, int winc, int btime, int binc, int movestogo)
{
/*	if (!process)
		return;
	QString cmd;
	QStringList list;
	ChessBoard b;
	int i;
	currentBoard = board;
	for (i = 0; i < moves.size; i++)
	{
		list.append(currentBoard.makeMoveText(moves.at(i),UCI).c_str());
		if (!currentBoard.doMove(moves.at(i), true))
			break;
	}
	string s=board.getFen();
	cmd = "position fen ";

	if (board.isStartposition())
		cmd += "startfen";
	else
		cmd += board.getFen(true).c_str();
	if (list.size())
	{
		cmd += " moves";
		QStringList::iterator lit = list.begin();
		while (lit != list.end())
		{
			cmd += " ";
			cmd += *lit;
			++lit;
		}
	}
	// If engine playing white it is most probaly not ready yet.
	if (readyok)
		write(cmd);
	else
		waitCommand += cmd;
	cmd="go";
	if (searchtype == PONDER_SEARCH)
		cmd += " ponder";
	if (searchtype == INFINITE_SEARCH)
	{
		cmd += " infinite";
	}
	else
	{
		cmd += " wtime " + QString().setNum(wtime);
		cmd += " btime " + QString().setNum(btime);
		cmd += " winc " + QString().setNum(winc);
		cmd += " binc " + QString().setNum(binc);
	}
	if (movestogo)
		cmd += "movestogo " + QString().setNum(movestogo);
	cmd += "\n";
	if (readyok)
		write(cmd);
	else
		waitCommand += cmd;
*/}

void Engine::stop()
{
	if (xboard)
		xboard->stop();
	else if (uci)
		uci->stop();
}

void Engine::setMultiPV(int n)
{
	if (xboard)
		xboard->setMultiPV(n);
	else if (uci)
		uci->setMultiPV(n);
}

bool Engine::isLoaded()
{
	if (xboard || uci)
		return true;
	return false;
}

bool Engine::needRestart()
{
	if (xboard)
		return xboard->needRestart();
	return false;
}

void Engine::newGame()
{
	if (xboard)
		xboard->newGame();
	else if (uci)
		uci->newGame();
}