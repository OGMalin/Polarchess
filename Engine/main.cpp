// Startup file for the engine

#include "FrontEnd.h"

using namespace std;

int main(int argc, char* argv[])
{
	// Create the main interface.
	FrontEnd fe;

	// Start the main loop.
	return fe.run();
}

