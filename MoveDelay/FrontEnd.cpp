#include "FrontEnd.h"
#include <string>
#include "..\Common\Utility.h"
//#include <iostream>

// Use with logging
//#include <fstream>
//std::ofstream logfileF;
//#define LOG(txt) logfileF.open("MoveDelay.log", ios::out | ios::app);logfileF << txt << endl;logfileF.close();
// Else
#define LOG(txt) {};

using namespace std;

FrontEnd::FrontEnd()
{
	delay = DELAY_normal;
	bookmove = false;
	wtime = btime = 0;
	stoping = true;
}

FrontEnd::~FrontEnd()
{

}

// The main loop of the driver.
int FrontEnd::run()
{
	HANDLE hs[3];
	DWORD dw;
	hs[0] = gui.hEvent;
	hs[1] = engine.hEvent;
	hs[2] = search.hEvent;

	//char sz[256];
	//GetCurrentDirectory(256, sz);
	//LOG(sz);
	if (!engine.start(enginepath))
	{
		LOG("Engine not started");
		gui.write("info Engine not started.");
		return 1;
	}
	while (1)
	{
		dw = WaitForMultipleObjects(3, hs, FALSE, INFINITE);

		switch (dw)
		{
		case WAIT_OBJECT_0:
			if (guiInput())
			{ // Exit recived
				engine.stop();
				return 0;
			}
			break;
		case WAIT_ABANDONED_0:
			break;
		case WAIT_OBJECT_0 + 1:
			engineInput();
			break;
		case WAIT_ABANDONED_0 + 1:
			break;
		case WAIT_OBJECT_0 + 2:
			searchInput();
			break;
		case WAIT_ABANDONED_0 + 2:
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
	size_t st;
	int i;
	string input, s, smove;
	int guiCmd;
	map<string, string>::iterator transIt;
	while ((guiCmd = gui.get(input)) != GUI_none)
	{
		transIt = transFromGui.begin();
		while (transIt != transFromGui.end())
		{
			st = input.find((*transIt).first);
			while (st != string::npos)
			{
				input.replace(st, (*transIt).first.length(), (*transIt).second);
				st = input.find((*transIt).first, st + (*transIt).second.length());
			}
			++transIt;
		}
		switch (guiCmd)
		{
		case GUI_quit:
			engine.write("quit");
			return true;
		case GUI_other:
			engine.write(input);
			break;
		case GUI_go:
			// Check for internal book moves
			if (polyglot.isOpen)
			{
				std::vector<PolyglotDataEntry> polymoves;
				polyglot.get(currentBoard, polymoves);
				if (polymoves.size() > 0)
				{

				}
			}
			bookmove = false;
			stoping = false;
			watch.start();
			wtime = btime = 0;
			if ((st = input.find("wtime ")) != string::npos)
				wtime = atoi(input.substr(st+6).c_str());
			if ((st = input.find("btime ")) != string::npos)
				btime = atoi(input.substr(st+6).c_str());
			engine.write("go " + input);
			break;
		case GUI_setoption:
			if (getWord(input, 2) == "Movedelay")
			{
				if (getWord(input, 4) == "None")
					delay = DELAY_none;
				else if (getWord(input, 4) == "Fast")
					delay = DELAY_fast;
				else if (getWord(input, 4) == "Normal")
					delay = DELAY_normal;
				else if (getWord(input, 4) == "Slow")
					delay = DELAY_slow;
				break;
			}
			engine.write("setoption " + input);
			break;
		case GUI_stop:
			stoping = true;
			engine.write("stop ");
			break;
		case GUI_position:
			currentBoard.setStartposition();
			if (getWord(input, 1) == "fen ")
			{
				st = input.find("moves");
				if (st != string::npos)
					st -= 4;
				currentBoard.setFen(input.substr(4, st).c_str());
			}
			st = input.find("moves ");
			if (st != string::npos)
			{
				s = input.substr(st + 6);
				i = 1;
				while (!(smove=getWord(s, i++)).empty())
					currentBoard.doMove(smove.c_str());
			}
			engine.write("position " + input);
			break;
		case GUI_test:
			//gui.input("setoption name UCI_LimitStrength value true");
			//gui.input("setoption name UCI_Elo value 1400");
			//gui.input("setoption name OwnBook value false");
			//gui.input("position startpos moves g1f3 g8f6 g2g3 b7b6 f1g2 c8b7 e1g1 e7e6");
			//gui.input("go wtime 940000 btime 932857 winc 10000 binc 10000");
//			ChessBoard cb;
////			cb.setStartposition();
//			cb.setFen("3q2k1/pp3pp1/5n1p/1NP5/1PN3b1/P2R4/4r1pP/6K1 w - - 0 29");
//			ChessMove m;
//			m=cb.getMoveFromText("d3d8");
//			watch.start();
//			search.easyMove(cb, m);
//			cout << watch.read(WatchPrecision::Millisecond) << endl;
			engine.write("test " + input);
			break;
		}
	}
	return false;
}

void FrontEnd::engineInput()
{
	string input;
	string bestmove, ponder;
	int engineResponse;
	size_t st;
	map<string, string>::iterator transIt;
	while ((engineResponse = engine.get(input)) != ENGINE_none)
	{
		transIt = transFromEngine.begin();
		while (transIt != transFromEngine.end())
		{
			st = input.find((*transIt).first);
			while (st != string::npos)
			{
				input.replace(st, (*transIt).first.length(), (*transIt).second);
				st = input.find((*transIt).first, st + (*transIt).second.length());
			}
			++transIt;
		}
		switch (engineResponse)
		{
		case ENGINE_other:
			gui.write(input);
			break;
		case ENGINE_uciok:
			gui.write("option name Movedelay type combo default Normal var None var Fast var Normal var Slow");
			gui.write("uciok");
			break;
		case ENGINE_info:
			if (input.find("book")!=string::npos)
				bookmove = true;
			gui.write("info " + input);
			break;
		case ENGINE_bestmove:
			// Don't delay bookmoves
			if (bookmove)
			{
				gui.write("bestmove " + input);
				break;
			}
			// Don't delay on analysis
			if ((wtime + btime) == 0)
			{
				gui.write("bestmove " + input);
				break;
			}
			// Don't delay first move for white.
			if (currentBoard.isStartposition())
			{
				gui.write("bestmove " + input);
				break;
			}
			// Don't delay if stoping
			if (stoping)
			{
				gui.write("bestmove " + input);
				break;
			}
			// Don't delay if not instant mover
			if (watch.read(WatchPrecision::Millisecond) > 5000)
			{
				gui.write("bestmove " + input);
				break;
			}
			if (search.checkMove(currentBoard, wtime, btime, getWord(input, 1), getWord(input, 3),delay, watch))
				gui.write("bestmove " + input);
			break;
		}
	}
}

void FrontEnd::searchInput()
{
	string input;
	int searchResponse;
	while ((searchResponse = search.get(input)) != SEARCH_none)
		gui.write(input);
}

void FrontEnd::transEngine(string ini, char* sz)
{
	char value[256];
	size_t index=0,st;
	string skey, svalue;
	while (sz[index] != 0)
	{
		if (GetPrivateProfileString("TransEngine", &sz[index], "", value, 256, ini.c_str()))
		{
			skey = &sz[index];
			svalue = value;
			st = svalue.find("\\n");
			while (st != string::npos)
			{
				svalue.replace(st, 2, "\n");
				st = svalue.find("\\n", st + 1);
			}
			transFromEngine.insert(map<string, string>::value_type(skey, svalue));
			index += skey.length() + 1;
		}
	}
}

void FrontEnd::transGui(string ini, char* sz)
{
	char value[256];
	size_t index = 0,st;
	string skey, svalue;
	while (sz[index] != 0)
	{
		if (GetPrivateProfileString("TransGui", &sz[index], "", value, 256, ini.c_str()))
		{
			skey = &sz[index];
			svalue = value;
			st = svalue.find("\\n");
			while (st != string::npos)
			{
				svalue.replace(st, 2, "\n");
				st = svalue.find("\\n", st + 1);
			}
			transFromGui.insert(map<string, string>::value_type(skey, svalue));
			index += skey.length() + 1;
		}
	}
}