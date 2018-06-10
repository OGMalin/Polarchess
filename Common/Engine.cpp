#include <QProcess>
#include "Engine.h"

#include <string>

#include "../Common/Utility.h"

using namespace std;

Engine::Engine()
{
	process = NULL;
	readyok = false;
}

Engine::~Engine()
{
	unload();
}

void Engine::setEngine(QString& name, QString& dir)
{
	engineName = name;
	workingDir = dir;
}

bool Engine::load(QString& setup)
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

void Engine::unload()
{
	if (!process)
		return;
	process->write("quit\n");
	if (process)
		process->waitForFinished(1000);
	delete process;
	process = NULL;
}

const QString Engine::lastError()
{
	return lasterror;
}

void Engine::slotErrorOccurred(QProcess::ProcessError e)
{
	char sz[16];
	QString err = tr("Error message: ");
	err.append(itoa(e, sz, 10));
	emit engineMessage(err);
}

void Engine::slotFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
}

void Engine::slotReadyStandardOutput()
{
	char sz[1024];
	string input, cmd, value;
	__int64 readlen;
	while (process && process->canReadLine())
	{
		readlen = process->readLine(sz, 1024);
		if (readlen)
		{
			input = trim(sz);
			cmd = getWord(input, 1);

			if (cmd == "uciok")
			{
				if (!setup.isEmpty())
					process->write(setup.toLatin1());
				process->write("isready\n");
			}
			else if (cmd == "readyok")
			{
				if (!waitCommand.isEmpty())
				{
					process->write(waitCommand.toLatin1());
					waitCommand.clear();
				}
				readyok = true;
				emit engineReady();
			}
			else if (cmd == "bestmove")
			{
				emit engineMove(QString(getWord(input, 2).c_str()), QString(getWord(input, 4).c_str()));
			}
		}
	}
}

void Engine::slotStarted()
{
	emit engineMessage(tr("Engine started"));
	process->write("uci\n");
}

void Engine::slotStateChanged(QProcess::ProcessState newState)
{

}

void Engine::slotReadyRead()
{
}

void Engine::write(QString& cmd)
{
	process->write(cmd.toLatin1());
	process->write("\n");
}

void Engine::search(QChessGame* game, SEARCHTYPE searchtype, int wtime, int winc, int btime, int binc, int movestogo)
{
	QString cmd;
	QStringList list;
	game->getMovelist(list,UCI);
	string s=game->getStartPosition().board().getFen();
	cmd = "position fen ";
	if (s == STARTFEN)
		cmd += "startfen";
	else
		cmd += s.c_str();
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
	cmd += "\n";
	// If engine playing white it is most probaly not ready yet.
	if (readyok)
		process->write(cmd.toLatin1());
	else
		waitCommand += cmd;
	cmd="go";
	if (searchtype == PONDER_SEARCH)
		cmd += " ponder";
	cmd += " wtime " + QString().setNum(wtime);
	cmd += " btime " + QString().setNum(btime);
	cmd += " winc " + QString().setNum(winc);
	cmd += " binc " + QString().setNum(binc);
	if (movestogo)
		cmd += "movestogo " + QString().setNum(movestogo);
	cmd += "\n";
	if (readyok)
		process->write(cmd.toLatin1());
	else
		waitCommand += cmd;
}