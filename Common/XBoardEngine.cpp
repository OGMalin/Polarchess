#include "../Common/XBoardEngine.h"
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

int XBoardEngine::readLine(std::string& line, QString& responsestring)
{
	responsestring.clear();
	string cmd;
	int response = DO_NOTHING;
	int index = 0;
	line = trim(line);
	cmd = getWord(line, 1);
	if (cmd == "feature")
	{
		index = cmd.size();
		if (index>=line.size())
			return response;
		line = trim(line.substr(index+1));
		response = readFeature(line, responsestring);
	}
	else if (cmd == "pong")
	{
		response = READYOK;
	}
	return response;
}

int XBoardEngine::isReady(QString& rs)
{
	if (feature.ping)
	{
		rs = "ping 1";
		return SEND_ENGINE;
	}
	return READYOK;

}

int XBoardEngine::readFeature(std::string& line, QString& responsestring)
{
	int index;
	int i;
	string cmd;
	string sub;
	string value;
	int response = DO_NOTHING;

	while ((cmd = getWord(line, 1, "=")).size() > 0)
	{
		if (cmd == "ping")
		{
			feature.ping = booleanString(line.substr(5, 1));
			responsestring += "accepted ping\n";
			index = 7;
		}
		else if (cmd == "setboard")
		{
			feature.setboard = booleanString(line.substr(9, 1));
			responsestring += "accepted setboard\n";
			index = 11;
		}
		else if (cmd == "playother")
		{
			feature.playother = booleanString(line.substr(10, 1));
			responsestring += "accepted playother\n";
			index = 12;
		}
		else if (cmd == "san")
		{
			feature.san = booleanString(line.substr(4, 1));
			responsestring += "accepted san\n";
			index = 6;
		}
		else if (cmd == "usermove")
		{
			feature.usermove = booleanString(line.substr(9, 1));
			responsestring += "accepted usermove\n";
			index = 11;
		}
		else if (cmd == "time")
		{
			feature.time = booleanString(line.substr(5, 1));
			responsestring += "accepted time\n";
			index = 7;
		}
		else if (cmd == "draw")
		{
			feature.draw = booleanString(line.substr(5, 1));
			responsestring += "accepted draw\n";
			index = 7;
		}
		else if (cmd == "sigint")
		{
			feature.sigint = booleanString(line.substr(7, 1));
			responsestring += "accepted sigint\n";
			index = 9;
		}
		else if (cmd == "sigterm")
		{
			feature.sigterm = booleanString(line.substr(8, 1));
			responsestring += "accepted sigterm\n";
			index = 10;
		}
		else if (cmd == "reuse")
		{
			feature.reuse = booleanString(line.substr(6, 1));
			responsestring += "accepted reuse\n";
			index = 8;
		}
		else if (cmd == "analyze")
		{
			feature.analyze = booleanString(line.substr(8, 1));
			responsestring += "accepted analyze\n";
			index = 10;
		}
		else if (cmd == "colors")
		{
			feature.colors = booleanString(line.substr(7, 1));
			responsestring += "accepted colors\n";
			index = 9;
		}
		else if (cmd == "ics")
		{
			feature.ics = booleanString(line.substr(4, 1));
			responsestring += "accepted ics\n";
			index = 6;
		}
		else if (cmd == "name")
		{
			feature.name = booleanString(line.substr(5, 1));
			responsestring += "accepted name\n";
			index = 7;
		}
		else if (cmd == "pause")
		{
			feature.pause = booleanString(line.substr(6, 1));
			responsestring += "accepted pause\n";
			index = 8;
		}
		else if (cmd == "nps")
		{
			feature.nps = booleanString(line.substr(4, 1));
			responsestring += "accepted nps\n";
			index = 6;
		}
		else if (cmd == "debug")
		{
			feature.debug = booleanString(line.substr(6, 1));
			responsestring += "accepted debug\n";
			index = 8;
		}
		else if (cmd == "memory")
		{
			feature.memory = booleanString(line.substr(7, 1));
			responsestring += "accepted memory\n";
			index = 9;
		}
		else if (cmd == "smp")
		{
			feature.smp = booleanString(line.substr(4, 1));
			responsestring += "accepted smp\n";
			index = 6;
		}
		else if (cmd == "done")
		{
			if (booleanString(line.substr(5, 1)))
				response = FINNISH_STARTUP;
			else
				response=WAIT_FOREVER;
			responsestring += "accepted done\n";
			index = 7;
		}
		else if (cmd == "myname")
		{
			feature.myname = getQuotedString(line.substr(7)).c_str();
			responsestring += "accepted myname\n";
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
			responsestring += "accepted variants\n";
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
			responsestring += "accepted egt\n";
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
			responsestring += "accepted option\n";
			index = 7 + 3 + value.length();
		}
		else
		{
			responsestring += "rejected ";
			responsestring += cmd.c_str();
			responsestring += "\n";
			index += cmd.size() + 1;
		}

		if (index >= line.size())
			return response;
		line = line.substr(index);
	}
	if (!responsestring.isEmpty())
		if (response == DO_NOTHING)
			response = SEND_ENGINE;

	return response;
}