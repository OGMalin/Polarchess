#include "../Common/UciEngine.h"
#include "../Common/Utility.h"

using namespace std;

UciEngine::UciEngine()
	:BaseEngine()
{
}

UciEngine::~UciEngine()
{
	if (process)
	{
		write("stop");
		_sleep(500);
		write("quit");
	}
}

void UciEngine::slotStarted()
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
		QString qs;
		QMap<QString, QString>::iterator it = initOptions.begin();
		while (it != initOptions.end())
		{
			if (!it.value().isEmpty())
			{
				qs = "setoption name " + it.key() +" value " + it.value();
				write(qs);
			}
			++it;
		}

		write("isready");
	}
	else if (cmd == "readyok")
	{
		emit engineStarted();
	}
	else if (cmd == "bestmove")
	{
		emit engineMove(QString(getWord(input, 2).c_str()), QString(getWord(input, 4).c_str()));
	}
	else if (cmd == "option")
	{
		EngineOption eo;
		int index = 2;
		string value = getWord(input, index);
		++index;
		while (value.length())
		{
			if (value == "name")
			{
				eo.name = getWord(input, index).c_str();
				value = getWord(input, index);
				while (!value.empty())
				{
					if (value == "type")
						break;
					eo.name += " ";
					eo.name += value.c_str();
					++index;
					value = getWord(input, index);
				}
				--index;
			}
			else if (value == "type")
			{
				value = getWord(input, index);
				if (value == "check")
					eo.type = OPTION_TYPE::CHECK;
				else if (value=="spin")
					eo.type = OPTION_TYPE::SPIN;
				else if (value == "combo")
					eo.type = OPTION_TYPE::COMBO;
				else if (value == "button")
					eo.type = OPTION_TYPE::BUTTON;
				else if (value == "string")
					eo.type = OPTION_TYPE::STRING;
			}
			else if (value == "default")
			{
				if (eo.type == OPTION_TYPE::CHECK)
				{
					eo.check.default = booleanString(getWord(input, index));
					eo.check.value = eo.check.default;
				}
				else if (eo.type == OPTION_TYPE::SPIN)
				{
					eo.spin.default = stoi(getWord(input, index));
					eo.spin.value = eo.spin.default;
				}
				else if (eo.type == OPTION_TYPE::COMBO)
				{
					eo.combo.default = getWord(input, index).c_str();
					eo.combo.value = eo.combo.default;
				}
				else if (eo.type == OPTION_TYPE::STRING)
				{
					eo.string.default= getWord(input, index).c_str();
					eo.string.value = eo.string.default;
				}
			}
			else if (value == "min")
			{
				eo.spin.min= stoi(getWord(input, index));
			}
			else if (value == "max")
			{
				eo.spin.max = stoi(getWord(input, index));
			}
			else if (value == "var")
			{
				eo.combo.var.push_back(getWord(input, index).c_str());
			}
			++index;
			value = getWord(input, index);
			++index;
		}
		engineOption.push_back(eo);
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
				ei.nodes = strtoul(getWord(input, index).c_str(), NULL, 0);
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
				ei.nps = strtoul(getWord(input, index).c_str(), NULL, 0);
			}
			else if (info == "tbhits")
			{
				ei.tbhits = strtoul(getWord(input, index).c_str(), NULL, 0);
			}
			else if (info == "sbhits")
			{
				ei.sbhits = strtoul(getWord(input, index).c_str(), NULL, 0);
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
}

void UciEngine::analyze(ChessBoard& board)
{
	int i;
	QString qs;

	// Stop the engine if needed
	stop();

	currentBoard=board;

	qs = "position ";

	if (currentBoard.isStartposition())
		qs += "startpos";
	else
		qs += QString("fen ") + currentBoard.getFen(true).c_str();

	write(qs);
	write("go infinite");
	searchtype = INFINITE_SEARCH;
}

void UciEngine::stop()
{
	// Stop current search
	if (searchtype != NO_SEARCH)
		write("stop");
	searchtype = NO_SEARCH;
}

void UciEngine::slotFinishInit()
{

}

void UciEngine::newGame()
{
	stop();
}

void UciEngine::setMultiPV(int n)
{
	int i;
	if (n < 1)
		return;
	for (i = 0; i < engineOption.size(); i++)
	{
		if (engineOption[i].name == "MultiPV")
		{
			if (engineOption[i].spin.value == n)
				return;
			if (engineOption[i].spin.max >= n)
			{
				QString qs = "setoption name multipv value ";
				qs += QString().setNum(n);
				write(qs);
			}
			return;
		}
	}
}