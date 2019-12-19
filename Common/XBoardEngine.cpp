#include "../Common/XBoardEngine.h"
#include "../Common/Utility.h"

using namespace std;

enum ENGINERESPONSE
{
	DO_NOTHING = 0,
	SEND_ENGINE,
	SEND_GUI_MESSAGE,
	SEND_GUI_MOVE,
	SEND_GUI_INFO
};

XBoardEngine::XBoardEngine()
{
	clear();
}

void XBoardEngine::clear()
{
	feature.ping = false; 
	feature.setboard = false;
	feature.playother = false;
	feature.san = false;
	feature.usermove = false;
	feature.time = true;
	feature.draw = true;
	feature.sigint = true;
	feature.sigterm = true;
	feature.reuse = true;
	feature.analyze = true;
	feature.myname.clear();
	feature.variants.clear();
	feature.colors = true;
	feature.ics = false;
	feature.name = false;
	feature.pause = false;
	feature.nps = false;
	feature.debug = false;
	feature.memory = false;
	feature.smp = false;
	feature.egt.clear();
	feature.option.clear();
}

int XBoardEngine::readLine(std::string& line, QString& response)
{
	string cmd;
	string sub;
	string value;
	int index = 0;
	int i;
	line = trim(line);
	cmd = getWord(line, 1);
	if (cmd == "feature")
	{
		index = cmd.size();
		if (index>=line.size())
			return DO_NOTHING;
		line = trim(line.substr(index));
		
		while ((cmd = getWord(line, 1, "=")).size() > 0)
		{
			if (cmd == "ping")
			{
				feature.ping = booleanString(line.substr(5, 1));
				index = 7;
			}
			else if (cmd == "setboard")
			{
				feature.setboard = booleanString(line.substr(9, 1));
				index = 11;
			}
			else if (cmd == "playother")
			{
				feature.playother = booleanString(line.substr(10, 1));
				index = 12;
			}
			else if (cmd == "san")
			{
				feature.san = booleanString(line.substr(4, 1));
				index = 6;
			}
			else if (cmd == "usermove")
			{
				feature.usermove = booleanString(line.substr(9, 1));
				index = 11;
			}
			else if (cmd == "time")
			{
				feature.time = booleanString(line.substr(5, 1));
				index = 7;
			}
			else if (cmd == "draw")
			{
				feature.draw = booleanString(line.substr(5, 1));
				index = 7;
			}
			else if (cmd == "sigint")
			{
				feature.sigint = booleanString(line.substr(7, 1));
				index = 9;
			}
			else if (cmd == "sigterm")
			{
				feature.sigterm = booleanString(line.substr(8, 1));
				index = 10;
			}
			else if (cmd == "reuse")
			{
				feature.reuse = booleanString(line.substr(6, 1));
				index = 9;
			}
			else if (cmd == "analyze")
			{
				feature.analyze = booleanString(line.substr(8, 1));
				index = 10;
			}
			else if (cmd == "colors")
			{
				feature.colors = booleanString(line.substr(7, 1));
				index = 9;
			}
			else if (cmd == "ics")
			{
				feature.ics = booleanString(line.substr(4, 1));
				index = 6;
			}
			else if (cmd == "name")
			{
				feature.name = booleanString(line.substr(5, 1));
				index = 7;
			}
			else if (cmd == "pause")
			{
				feature.pause = booleanString(line.substr(6, 1));
				index = 8;
			}
			else if (cmd == "nps")
			{
				feature.nps = booleanString(line.substr(4, 1));
				index = 6;
			}
			else if (cmd == "debug")
			{
				feature.debug = booleanString(line.substr(6, 1));
				index = 8;
			}
			else if (cmd == "memory")
			{
				feature.memory = booleanString(line.substr(7, 1));
				index = 9;
			}
			else if (cmd == "smp")
			{
				feature.smp = booleanString(line.substr(4, 1));
				index = 6;
			}
			else if (cmd == "myname")
			{
				feature.myname = getQuotedString(line.substr(7)).c_str();
				index = 7+3+feature.myname.length();
			}
			else if (cmd == "variants")
			{
				value= getQuotedString(line.substr(9));
				i = 1;
				while ((sub = getWord(value, i, ",")).length())
				{
					feature.variants.push_back(sub.c_str());
					++i;
				}
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
				index = 7 + 3 + value.length();
			}

			if (index >= line.size())
				return DO_NOTHING;
			line = line.substr(index);
		}
		
		return DO_NOTHING;
	}
	return DO_NOTHING;
}
