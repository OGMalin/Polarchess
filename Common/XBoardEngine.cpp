#include "../Common/XBoardEngine.h"
#include "../Common/Utility.h"

using namespace std;

XBoardEngine::XBoardEngine()
	:BaseEngine()
{
	feature.sigint = true;
	feature.sigterm = true;
	feature.ping = false;
	feature.setboard = false;
	feature.memory = false;
	feature.smp = false;
	feature.reuse = true;
	feature.usermove = false;
	feature.debug = false;
	feature.draw = true;
	feature.pause = false;
	feature.nps = true;
	feature.analyze = true;
	feature.exclude = false;
	feature.setscore = false;
	feature.highlight = false;
	feature.playother = false;
	feature.ics = false;
	feature.name = false;
	feature.colors = true;
	feature.time = true;
	feature.san = false;
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), SLOT(slotFinishInit()));
}

XBoardEngine::~XBoardEngine()
{
	unload();
	delete timer;
}

bool XBoardEngine::load(QString& path)
{
	if (!BaseEngine::load(path))
		return false;
	restartNeeded = false;
	return true;
}

void XBoardEngine::unload()
{
	if (process)
	{
		if (searchtype != NO_SEARCH)
		{
			if (searchtype == INFINITE_SEARCH)
				write("exit");
			else
				write("?");
			_sleep(500);
		}
		write("quit");
		BaseEngine::unload();
	}
}

void XBoardEngine::fromEngine(std::string& input)
{
	string cmd;
	string line;
	int i = 0;
	line = trim(input);
	cmd = getWord(line, 1);
	if (cmd.empty())
		return;
	if (cmd == "feature")
	{
		i = cmd.size();
		if (i>= line.size())
			return;
		line = trim(line.substr(i+1));
		readFeature(line);
	}
	else if (cmd.at(0) == '#') // Comment
	{
		return;
	}
	else if (isNumber(cmd)) // Check if it is a pv line
	{
		EngineInfo ei;
		i = 1;
		ei.depth = atoi(cmd.c_str());
		cmd = getWord(input, ++i);
		if (isNumber(cmd))
		{
			ei.cp = atoi(cmd.c_str());
			cmd = getWord(input, ++i);
			if (isNumber(cmd))
			{
				ei.time = strtoul(cmd.c_str(), NULL, 0) * 10;
				cmd = getWord(input, ++i);
				if (isNumber(cmd))
				{
					ei.nodes = strtoul(cmd.c_str(), NULL, 0);

					// Read pv
					ChessBoard cb = currentBoard;
					ChessMove m;
					while ((cmd = getWord(input, ++i)).size())
					{
						m = cb.getMoveFromText(cmd);
						if (!m.empty())
							if (cb.doMove(m, true))
								ei.pv.push_back(m);
					}
					emit engineInfo(ei);
				}
			}
		}
	}
}

void XBoardEngine::readFeature(std::string& line)
{
	int index;
	int i;
	string cmd;
	string sub;
	string value;

	while ((cmd = getWord(line, 1, "=")).size() > 0)
	{
		if (cmd == "ping")
		{
			feature.ping = booleanString(line.substr(5, 1));
			write("accepted ping");
			index = 7;
		}
		else if (cmd == "setboard")
		{
			feature.setboard = booleanString(line.substr(9, 1));
			write("accepted setboard");
			index = 11;
		}
		else if (cmd == "playother")
		{
			feature.playother = booleanString(line.substr(10, 1));
			write("accepted playother");
			index = 12;
		}
		else if (cmd == "san")
		{
			feature.san = booleanString(line.substr(4, 1));
			write("accepted san");
			index = 6;
		}
		else if (cmd == "usermove")
		{
			feature.usermove = booleanString(line.substr(9, 1));
			write("accepted usermove");
			index = 11;
		}
		else if (cmd == "time")
		{
			feature.time = booleanString(line.substr(5, 1));
			write("accepted time");
			index = 7;
		}
		else if (cmd == "draw")
		{
			feature.draw = booleanString(line.substr(5, 1));
			write("accepted draw");
			index = 7;
		}
		else if (cmd == "sigint")
		{
			feature.sigint = booleanString(line.substr(7, 1));
			write("accepted sigint");
			index = 9;
		}
		else if (cmd == "sigterm")
		{
			feature.sigterm = booleanString(line.substr(8, 1));
			write("accepted sigterm");
			index = 10;
		}
		else if (cmd == "reuse")
		{
			feature.reuse = booleanString(line.substr(6, 1));
			write("accepted reuse");
			index = 8;
		}
		else if (cmd == "analyze")
		{
			feature.analyze = booleanString(line.substr(8, 1));
			write("accepted analyze");
			index = 10;
		}
		else if (cmd == "colors")
		{
			feature.colors = booleanString(line.substr(7, 1));
			write("accepted colors");
			index = 9;
		}
		else if (cmd == "ics")
		{
			feature.ics = booleanString(line.substr(4, 1));
			write("accepted ics");
			index = 6;
		}
		else if (cmd == "name")
		{
			feature.name = booleanString(line.substr(5, 1));
			write("accepted name");
			index = 7;
		}
		else if (cmd == "pause")
		{
			feature.pause = booleanString(line.substr(6, 1));
			write("accepted pause");
			index = 8;
		}
		else if (cmd == "nps")
		{
			feature.nps = booleanString(line.substr(4, 1));
			write("accepted nps");
			index = 6;
		}
		else if (cmd == "debug")
		{
			feature.debug = booleanString(line.substr(6, 1));
			write("accepted debug");
			index = 8;
		}
		else if (cmd == "memory")
		{
			feature.memory = booleanString(line.substr(7, 1));
			write("accepted memory");
			index = 9;
		}
		else if (cmd == "smp")
		{
			feature.smp = booleanString(line.substr(4, 1));
			write("accepted smp");
			index = 6;
		}
		else if (cmd == "done")
		{	
			write("accepted done");
			if (booleanString(line.substr(5, 1)))
				slotFinishInit();
			index = 7;
		}
		else if (cmd == "exclude")
		{
			feature.exclude = booleanString(line.substr(8, 1));
			write("accepted exclude");
			index = 10;
		}
		else if (cmd == "setscore")
		{
			feature.setscore = booleanString(line.substr(9, 1));
			write("accepted setscore");
			index = 11;
		}
		else if (cmd == "highlight")
		{
			feature.highlight = booleanString(line.substr(10, 1));
			write("accepted highlight");
			index = 12;
		}
		else if (cmd == "myname")
		{
			feature.myname = getQuotedString(line.substr(7)).c_str();
			write("accepted myname");
			index = 7 + 3 + feature.myname.length();
		}
		else if (cmd == "variants")
		{
			value = getQuotedString(line.substr(9));
			i = 1;
			while ((sub = getWord(value, i, ",")).length())
			{
				feature.variants.push_back(sub.c_str());
				++i;
			}
			write("accepted variants");
			index = 9 + 3 + value.length();
		}
		else if (cmd == "egt")
		{
			value = getQuotedString(line.substr(4));
			i = 1;
			while ((sub = getWord(value, i, ",")).length())
			{
				feature.egt.push_back(sub.c_str());
				++i;
			}
			write("accepted egt");
			index = 4 + 3 + value.length();
		}
		else if (cmd == "option")
		{
			value = getQuotedString(line.substr(7));
			i = 1;
			while ((sub = getWord(value, i, ",")).length())
			{
				feature.option.push_back(sub.c_str());
				++i;
			}
			write("accepted option");
			index = 7 + 3 + value.length();
		}
		else
		{
			write(string("rejected " + cmd).c_str());
			index += cmd.size() + 1;
		}

		if (index >= line.size())
			return;
		line = line.substr(index);
	}
}

void XBoardEngine::slotFinishInit()
{
	timer->stop();
	emit engineStarted();
}

void XBoardEngine::analyze(ChessBoard& board)
{
	currentBoard = board;

	QString qs;

	if (!feature.analyze)
		return;

	if (!feature.reuse)
		restartNeeded = true;
	// Stop current search
	if (searchtype != NO_SEARCH)
	{
		if (searchtype == INFINITE_SEARCH)
			write("exit");
		else
			write("?");
	}

	write("force");
	if (feature.setboard)
	{
		qs = "setboard ";
		qs += currentBoard.getFen().c_str();
		write(qs);
	}else
	{
		int file, row, piece, i, j;
		char filechar[] = "abcdefgh";
		char rowchar[] = "12345678";
		char piecechar[] = "PNBRQK";
		int color;
		// XBoard edit command
		if (currentBoard.toMove == BLACK)
			write("a2a3");
		write("edit");
		write("#");
		color = currentBoard.toMove;
		for (file = 0; file < 8; file++)
		{
			for (row = 0; row < 8; row++)
			{
				piece = PIECE(currentBoard.pieceAt(file, row));
				if (piece != EMPTY)
				{
					qs = piecechar[piece - 1];
					qs += filechar[file];
					qs += rowchar[row];
					if (PIECECOLOR(currentBoard.pieceAt(file, row)) != color)
					{
						write("c");
						color = PIECECOLOR(currentBoard.pieceAt(file, row));
					}
					write(qs);
				}
			}
		}
		write(".");
	}
	write("post");
	write("analyze");
	searchtype = INFINITE_SEARCH;
}

void XBoardEngine::stop()
{
	// Stop current search
	switch (searchtype)
	{
	case NORMAL_SEARCH:
	case NODES_SEARCH:
	case MATE_SEARCH:
	case DEPTH_SEARCH:
	case TIME_SEARCH:
		write("?");
		break;
	case INFINITE_SEARCH:
		write("exit");
		break;
	}
	searchtype = NO_SEARCH;
}

void XBoardEngine::slotStarted()
{
	// XBoard engines should possible send options command before set in xboard mode, but ping/pong could possible not work then because
	// it is a xboard feature.
	QString qs;
	QMap<QString, QString>::iterator it = initOptions.begin();
	while (it != initOptions.end())
	{
		qs = it.key();
		if (!it.value().isEmpty())
		{
			qs += " ";
			qs += it.value();
		}
		write(qs);
		++it;
	}
	write("xboard");
	write("protover 2");

	// Wait 2 sek if the engine don't have done 1
	timer->setInterval(2000);
	timer->setSingleShot(true);
	timer->start();
}

void XBoardEngine::slotFinished()
{
	BaseEngine::slotFinished();
}

bool XBoardEngine::needRestart()
{
	return restartNeeded;
}

void XBoardEngine::newGame()
{
	stop();
}