#include "../Common/XBoardEngine.h"
#include "../Common/Utility.h"
#include <QTimer>

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
}

void XBoardEngine::fromEngine(std::string& input)
{
	string cmd;
	string line;
	int index = 0;
	line = trim(input);
	cmd = getWord(line, 1);
	if (cmd == "feature")
	{
		index = cmd.size();
		if (index>= line.size())
			return;
		line = trim(line.substr(index+1));
		readFeature(line);
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
		else if (cmd == "times")
		{
			feature.time = booleanString(line.substr(6, 1));
			write("accepted times");
			index = 8;
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
			if (booleanString(line.substr(5, 1)))
				finnishInit();
			write("accepted done");
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

void XBoardEngine::started()
{
	// XBoard engines should possible send options command before set in xboard mode, but ping/pong could possible not work then because
	// it is a xboard feature.
	QString qs;
	QMap<QString, QString>::iterator it=initOptions.begin();
	while (it!=initOptions.end())
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

	// Wait 5 sek if the engine don't have done 1
	QTimer::singleShot(2000, this, SLOT(finnishInit()));
}

void XBoardEngine::finnishInit()
{
	static bool sendt = false;
	if (sendt)
		return;
	sendt = true;
	emit finnishStartup();
}