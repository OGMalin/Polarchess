// Startup file for the delay interface

#include "FrontEnd.h"
#include "../Common/Utility.h"
//#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{
	// Create the main interface.
	FrontEnd fe;

	string path = getProgramPath();
	SetCurrentDirectory(path.c_str());

	// Find engine path.
	if (argc > 1)
	{
		fe.enginepath = argv[1];
	}
	else
	{
		char sz[256];
		string s;
		s = path + "MoveDelay.ini";
		if (GetPrivateProfileString("Engine", "Path", "", sz, 256, s.c_str()))
			fe.enginepath = sz;
	}

	// Start the main loop.
	return fe.run();
}

