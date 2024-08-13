// Startup file for the delay interface

#include "FrontEnd.h"
#include "../Common/Utility.h"
#include <string>
//#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{
	char sz[4096];

	// Create the main interface.
	FrontEnd fe;

	string path = getProgramPath();
	SetCurrentDirectory(path.c_str());

	GetModuleFileName(NULL, sz, MAX_PATH);
	path += getFilename(sz);
	path += ".ini";

	// Find engine path.
	if (argc > 1)
	{
		fe.enginepath = argv[1];
	}
	else
	{
		if (GetPrivateProfileString("Engine", "Path", "", sz, 256, path.c_str()))
			fe.enginepath = sz;
	}

	if (GetPrivateProfileString("Engine", "Book", "", sz, 256, path.c_str()))
	{
		string b=sz;
		fe.polyglot.open(b);
	}

	if (GetPrivateProfileString("Engine", "BestMove", "", sz, 256, path.c_str()))
	{
		if ((strlen(sz)>0) && (sz[0]=='t'))
			fe.BestMove = true;
	}

	if (GetPrivateProfileString("TransEngine", NULL, "", sz, 4096, path.c_str()))
		fe.transEngine(path, sz);
	if (GetPrivateProfileString("TransGui", NULL, "", sz, 4096, path.c_str()))
		fe.transGui(path, sz);

	// Start the main loop.
	return fe.run();
}

