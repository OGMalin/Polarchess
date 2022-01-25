// Startup file for the delay interface

#include "FrontEnd.h"
//#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{
	// Create the main interface.
	FrontEnd fe;

	// Find engine path.
	if (argc > 1)
	{
		fe.enginepath = argv[1];
	}
	else
	{
		char sz[256];
		string s;
		GetCurrentDirectory(256, sz);
		s = sz;
		s+="\\MoveDelay.ini";
		if (GetPrivateProfileString("Engine", "Path", "", sz, 256, s.c_str()))
			fe.enginepath = sz;
	}

	// Start the main loop.
	return fe.run();
}

