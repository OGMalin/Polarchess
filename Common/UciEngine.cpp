#include "../Common/UciEngine.h"

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
{
	clear();
}

void UciEngine::clear()
{

}

int UciEngine::readLine(std::string& line, QString& responsestring)
{
	int response = DO_NOTHING;
	return response;
}
