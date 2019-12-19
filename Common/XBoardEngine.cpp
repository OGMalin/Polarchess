#include "../Common/XBoardEngine.h"

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

int XBoardEngine::readLine(QString& line)
{
	return DO_NOTHING;
}
