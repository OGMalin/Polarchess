#include <Windows.h>
#include "FrontEnd.h"

using namespace std;

FrontEnd::FrontEnd()
{
	debug = false;
}

FrontEnd::~FrontEnd()
{

}

// The main loop of the driver.
int FrontEnd::run()
{
	HANDLE hs[2];
	DWORD dw;
	hs[0] = gui.hEvent;
	hs[1] = dgt.hEvent;

	while (1)
	{
		dw = WaitForMultipleObjects(2, hs, FALSE, INFINITE);

		switch (dw)
		{
		case WAIT_OBJECT_0:
			if (guiInput())
			{ // Exit recived
				// Disconnect the board
				dgt.disconnectDgt();
				WaitForSingleObject(dgt.hThread, 5000);
				return 0;
			}
			break;
		case WAIT_ABANDONED_0:
			break;
		case WAIT_OBJECT_0 + 1:
			break;
		case WAIT_ABANDONED_0 + 1:
			break;
		case WAIT_TIMEOUT:
			break;
		default:
			break;
		}
	}
	return 0;
};

bool FrontEnd::guiInput()
{
	string input;
	int guiCmd;
	while ((guiCmd = gui.get(input)) != GUI_none)
	{
		switch (guiCmd)
		{
		case GUI_quit:
			return true;
		case GUI_unknown:
			gui.write(string("Error Unknown command: " + input));
			break;
		case GUI_eboard:
			gui.write(string("Driver PolarChess dgtdriver version 0.1"));
			break;
		case GUI_connect:
			dgt.connectDgt("com4");
			break;
		case GUI_disconnect:
			dgt.disconnectDgt();
			break;
		case GUI_debug:
			debug = debug ? false : true;
			break;
		}
	}
	return false;
}
