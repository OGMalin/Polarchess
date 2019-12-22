#include "../Common/UciEngine.h"
#include "../Common/Utility.h"

using namespace std;

enum ENGINERESPONSE
{
	DO_NOTHING = 0,
	SEND_ENGINE,
	SEND_GUI_MESSAGE,
	SEND_GUI_MOVE,
	SEND_GUI_INFO,
	FINNISH_STARTUP,
	WAIT_FOREVER,
	READYOK
};


UciEngine::UciEngine()
	:BaseEngine()
{
}

void UciEngine::started()
{
	write("uci");
}

void UciEngine::fromEngine(std::string& input)
{
	string cmd;
	string line;
	int index = 0;
	line = trim(input);
	cmd = getWord(line, 1);
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
	else if (cmd == "info")
	{
		EngineInfo ei;
		int index = 2;
		string info = getWord(input, index);
		++index;
		while (info.length())
		{
			if (info == "depth")
			{
				ei.depth = stoi(getWord(input, index));
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
				ei.string = getWord(input, index).c_str();
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
					ei.cpunr = stoi(s);
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
	else
	{
		switch (uci->readLine(input, responsestring))
		{
		case FINNISH_STARTUP:
			startup = false;
			break;
		}
	}
}

}
