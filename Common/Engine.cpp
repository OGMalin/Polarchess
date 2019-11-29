#include <QProcess>
#include "Engine.h"
#include "../Common/Utility.h"
#include <string>
#include <QSettings>
#include <QDebug>

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
	unload();
	engineName = name;
	workingDir = dir;
	QString path = dir + "/" + name;
}

bool Engine::load(QString& enginefile)
{
	if (process)
		return false;
	QSettings settings(enginefile, QSettings::IniFormat);
	settings.beginGroup("Engine");
	QString enginepath = settings.value("path").toString();
	if (enginepath.isEmpty())
		return false;
	settings.endGroup();
	settings.beginGroup("Option");
	QStringList keys = settings.allKeys();

	options.clear();
	QString val;
	for (int i = 0; i < keys.size(); i++)
	{
		val = settings.value(keys[i]).toString();
		if (!val.isEmpty())
			options.push_back(keys[i] + " value " + val);
	}
	settings.endGroup();

	process = new QProcess(this);
	if (!process)
		return false;

	connect(process, SIGNAL(errorOccurred(QProcess::ProcessError)), SLOT(slotErrorOccurred(QProcess::ProcessError)));
	connect(process, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(slotFinished(int, QProcess::ExitStatus)));
	connect(process, SIGNAL(readyReadStandardOutput()), SLOT(slotReadyStandardOutput()));
	connect(process, SIGNAL(started()), SLOT(slotStarted()));

	process->setReadChannel(QProcess::StandardOutput);

	emit engineMessage("Starting engine");
	process->start(enginepath);
	return true;
}

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

void Engine::unload()
{
	if (!process)
		return;
	disconnect(process);
	write("stop");
	_sleep(500);
	write("quit");
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
		qDebug(sz);
		if (readlen)
		{
			input = trim(sz);
			cmd = getWord(input, 1);

			if (cmd == "uciok")
			{
				if (!options.isEmpty())
				{
					for (int i = 0; i < options.size(); i++)
					{
						write(QString("setoption name " + options[i]));
					}
				}
				else if (!setup.isEmpty())
				{
					write(setup);
					write("isready");
				}
				write("isready");
			}
			else if (cmd == "readyok")
			{
				if (!waitCommand.isEmpty())
				{
					write(waitCommand);
					waitCommand.clear();
				}
				readyok = true;
				emit engineReady();
			}
			else if (cmd == "bestmove")
			{
				emit engineMove(QString(getWord(input, 2).c_str()), QString(getWord(input, 4).c_str()));
			}
			else if(cmd == "info")
			{
				EngineInfo ei;
				int index = 2;
				string info=getWord(input,index);
				++index;
				while (info.length())
				{
					if (info == "depth")
					{
						ei.depth=stoi(getWord(input, index));
					}
					else if (info == "seldepth")
					{
						ei.seldepth = stoi(getWord(input, index));
					}
					else if (info == "time")
					{
						ei.time = stoi(getWord(input, index));
					}
					else if (info == "nodes")
					{
						ei.nodes = stoi(getWord(input, index));
					}
					else if (info == "pv")
					{
						ChessBoard b = currentBoard;
						ChessMove m = b.getMoveFromText(getWord(input, index));
						while (!m.empty())
						{
							ei.pv.push_back(m);
							b.doMove(m, false);
							++index;
							m = b.getMoveFromText(getWord(input, index));
						}
						--index;
					}
					else if (info == "multipv")
					{
						ei.multipv = stoi(getWord(input, index));
					}
					else if (info == "score")
					{
						info = getWord(input, index);
						++index;
						if (info == "cp")
						{
							ei.cp = stoi(getWord(input, index));
							if (currentBoard.toMove == BLACK)
								ei.cp *= -1;
						}
						else if (info == "mate")
						{
							ei.mate = stoi(getWord(input, index));
							if (currentBoard.toMove == BLACK)
								ei.mate *= -1;
						}
					}
					else if (info == "currmove")
					{
						ei.currmove = currentBoard.getMoveFromText(getWord(input, index));
					}
					else if (info == "currmovenumber")
					{
						ei.currmovenumber = stoi(getWord(input, index));
					}
					else if (info == "hashfull")
					{
						ei.hashfull = stoi(getWord(input, index));
					}
					else if (info == "nps")
					{
						ei.nps = stoi(getWord(input, index));
					}
					else if (info == "tbhits")
					{
						ei.tbhits = stoi(getWord(input, index));
					}
					else if (info == "sbhits")
					{
						ei.sbhits = stoi(getWord(input, index));
					}
					else if (info == "cpuload")
					{
						ei.cpuload = stoi(getWord(input, index));
					}
					else if (info == "string")
					{
						ei.string=getWord(input,index).c_str();
						++index;
						string s = getWord(input, index);
						while (s.length())
						{
							ei.string += " ";
							ei.string += s.c_str();
							++index;
							s = getWord(input, index);
						}
						// string is the rest of the line so break out of the while loop.
						break;
					}
					else if (info == "refutation")
					{
						ChessBoard b = currentBoard;
						ChessMove m = b.getMoveFromText(getWord(input, index));
						while (!m.empty())
						{
							ei.refutation.push_back(m);
							b.doMove(m, false);
							++index;
							m = b.getMoveFromText(getWord(input, index));
						}
						--index;
					}
					else if (info == "currline")
					{
						string s = getWord(input, index).c_str();
						if (isNumber(s))
						{
							ei.cpunr= stoi(s);
							++index;
						}
						ChessBoard b = currentBoard;
						ChessMove m = b.getMoveFromText(s);
						while (!m.empty())
						{
							ei.refutation.push_back(m);
							b.doMove(m, false);
							++index;
							m = b.getMoveFromText(getWord(input, index));
						}
						--index;

					}

					++index;
					info = getWord(input, index);
					++index;
				}
				emit engineInfo(ei);
			}
		}
	}
}

void Engine::slotStarted()
{
	emit engineMessage(tr("Engine started"));
	write("uci");
}

void Engine::slotStateChanged(QProcess::ProcessState newState)
{

}

void Engine::slotReadyRead()
{
}

void Engine::write(QString& cmd)
{
	qDebug(cmd.toLatin1());
	if (!process)
		return;
	process->write(cmd.toLatin1());
	process->write("\n");
}

void Engine::search(ChessBoard& board, MoveList& moves, SEARCHTYPE searchtype, int wtime, int winc, int btime, int binc, int movestogo)
{
	if (!process)
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
}

void Engine::stop()
{
	write("stop");
}

void Engine::setMultiPV(int n)
{
	QString qs = "setoption name multipv value ";
	qs += QString().setNum(n);
	write(qs);
}
