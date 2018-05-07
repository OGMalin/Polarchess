#include <Windows.h>
#include <string>
#include "frontend.h"
#include "Utility.h"
#include "ChessBoard.h"
#include "MoveList.h"
#include "MoveGenerator.h"

using namespace std;

const char* ENGINENAME = "PolarChess 3.0";

FrontEnd::FrontEnd()
{
	debug = false;
	currentBoard.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

FrontEnd::~FrontEnd()
{

}


// The main loop of the engine.
int FrontEnd::run()
{
	HANDLE hs[2];
	DWORD dw;

	hs[0] = uci.hEvent;
	hs[1] = engine.hEvent;

	engine.send(ENG_clearhistory);

	while (1)
	{
		dw = WaitForMultipleObjects(2, hs, FALSE, INFINITE);

		switch (dw)
		{
		case WAIT_OBJECT_0:
			if (uciInput())
				return 0;
			break;
		case WAIT_ABANDONED_0:
			break;
		case WAIT_OBJECT_0+1:
			engineInput();
			if (uciInput())
				return 0;
			break;
		case WAIT_ABANDONED_0+1:
			break;
		case WAIT_TIMEOUT:
			break;
		default:
			break;
		}
	}
	return 0;
};

int FrontEnd::uciInput()
{
	string input;
	int uciCmd;
	while ((uciCmd = uci.get(input)) != UCI_none)
	{
		switch (uciCmd)
		{
		case UCI_uci:
			uciUci();
			break;
		case UCI_debug:
			uciDebug(input);
			break;
		case UCI_isready:
			uciIsready();
			break;
		case UCI_setoption:
			uciSetoption(input);
			break;
		case UCI_register:
			uciRegister(input);
			break;
		case UCI_ucinewgame:
			uciUcinewgame();
			break;
		case UCI_position:
			uciPosition(input);
			break;
		case UCI_go:
			uciGo(input);
			break;
		case UCI_stop:
			uciStop();
			break;
		case UCI_ponderhit:
			uciPonderhit();
			break;
		case UCI_quit:
			return true;
		case UCI_movegen:
			uciMovegen(input);
			break;
		case UCI_unknown:
			uci.write(string("info string Unknown command: " + input));
			break;
		}
	}
	return false;
}

/* uci
*
* tell engine to use the uci (universal chess interface), this will be sent
* once as a first command after program boot to tell the engine to switch to
* uci mode.
* After receiving the uci command the engine must identify itself with the "id"
* command and send the "option" commands to tell the GUI which engine settings
* the engine supports if any.
* After that the engine should send "uciok" to acknowledge the uci mode.
* If no uciok is sent within a certain time period, the engine task will be killed
* by the GUI.
*/
void FrontEnd::uciUci()
{
	uci.write("id name " + string(ENGINENAME));
	uci.write("id author Odd Gunnar Malin");
	uci.write("option name UCI_EngineAbout type string default http://polarchess.net");
	uci.write("uciok");
}

/* debug [ on | off ]
*
* switch the debug mode of the engine on and off.
* In debug mode the engine should send additional infos to the GUI, e.g. with
* the "info string"
*/
void FrontEnd::uciDebug(const std::string& s)
{
	if (s.length())
		debug = booleanString(s);
}

/* isready
*
* this is used to synchronize the engine with the GUI. When the GUI has sent a
* command or multiple commands that can take some time to complete, this command
* can be used to wait for the engine to be ready again or to ping the engine to
* find out if it is still alive.
* E.g. this should be sent after setting the path to the tablebases as this can
* take some time.
* This command is also required once before the engine is asked to do any search
* to wait for the engine to finish initializing.
* This command must always be answered with "readyok" and can be sent also when
* the engine is calculating in which case the engine should also immediately answer
* with "readyok" without stopping the search.
*/
void FrontEnd::uciIsready()
{
	uci.write("readyok");
}

/* setoption name <id> [value <x>]
*
* this is sent to the engine when the user wants to change the internal parameters
* of the engine. For the "button" type no value is needed.
* One string will be sent for each parameter and this will only be sent when the
* engine is waiting.
* The name and value of the option in <id> should not be case sensitive and can
* inlude spaces.
* The substrings "value" and "name" should be avoided in <id> and <x> to allow
* unambiguous parsing, for example do not use <name> = "draw value".
* Here are some strings for the example below:
*    "setoption name Nullmove value true\n"
*    "setoption name Selectivity value 3\n"
*    "setoption name Style value Risky\n"
*    "setoption name Clear Hash\n"
*    "setoption name NalimovPath value c:\chess\tb\4;c:\chess\tb\5\n"
*/
void FrontEnd::uciSetoption(const std::string& s)
{
	string name, value, ss;
	int i;

	i = 2;
	while ((ss = getWord(s, i)).length())
	{
		if (s == "value")
			break;
		name += lowercase(ss);
		++i;
	}

	while ((ss = getWord(s, i)).length())
	{
		value += ss;
		value += ' ';
		++i;
	}
}

/* register
*
* this is the command to try to register an engine or to tell the engine that
* registration will be done later. This command should always be sent if the engine
* has sent "registration error" at program startup.
* The following tokens are allowed:
*   * later
*     the user doesn't want to register the engine now.
*   * name <x>
*     the engine should be registered with the name <x>
*   * code <y>
*     the engine should be registered with the code <y>
* Example:
*   "register later"
*   "register name Stefan MK code 4359874324"
*/
void FrontEnd::uciRegister(const std::string& s)
{

}

/* ucinewgame
*
* this is sent to the engine when the next search (started with "position" and "go")
* will be from a different game. This can be a new game the engine should play or a
* new game it should analyse but also the next position from a testsuite with
* positions only.
* If the GUI hasn't sent a "ucinewgame" before the first "position" command, the
* engine shouldn't expect any further ucinewgame commands as the GUI is probably not
* supporting the ucinewgame command.
* So the engine should not rely on this command even though all new GUIs should
* support it.
* As the engine's reaction to "ucinewgame" can take some time the GUI should always
* send "isready" after "ucinewgame" to wait for the engine to finish its operation.
*/
void FrontEnd::uciUcinewgame()
{
	currentBoard.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	engine.send(ENG_clearhistory);
	engine.send(ENG_clearhash);
}

/* position [fen <fenstring> | startpos ]  moves <move1> .... <movei>
*
* set up the position described in fenstring on the internal board and play the
* moves on the internal chess board.
* If the game was played  from the start position the string "startpos" will be sent
* Note: no "new" command is needed. However, if this position is from a different
* game than the last position sent to the engine, the GUI should have sent a
* "ucinewgame" inbetween.
*/
void FrontEnd::uciPosition(const std::string& input)
{
	string cmd;
	ChessMove m;
	int i = 1, n;

	// Clear 3-rep table (m=empty move).
	engine.send(ENG_clearhistory);

	// Read the inputline
	cmd = getWord(input, i++);
	while (cmd.length()>0)
	{
		// Startposition as fen-string
		if (cmd == "fen")
		{
			cmd = getWord(input, i++);
			if ((getWord(input, i) == "w") || (getWord(input, i) == "b"))
			{
				cmd += ' ';
				cmd += getWord(input, i++);
				if (existIn(getWord(input, i).at(0), "KQkq-"))
				{
					cmd += ' ';
					cmd += getWord(input, i++);
					if (existIn(getWord(input, i).at(0), "abcdefgh"))
					{
						if (existIn(getWord(input, i).at(1), "36"))
						{
							cmd += ' ';
							cmd += getWord(input, i++);
						}
					}
				}
			}
			currentBoard.setFen(cmd.c_str());
		}
		else if (cmd == "startpos")
		{
			// Startposition as a normal gamestart.
			currentBoard.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
		}
		else if (cmd == "moves")
		{
			// Read the moves and update the position and drawtable.
			n = 0;
			cmd = getWord(input, i++);
			while (cmd.length())
			{
				engine.send(ENG_history, currentBoard);
				m = currentBoard.getMoveFromText(cmd);
				currentBoard.doMove(m, false);
				cmd = getWord(input, i++);
			}

		}
		cmd = getWord(input, i++);
	}
}

/* go
*
* start calculating on the current position set up with the "position" command.
* There are a number of commands that can follow this command, all will be sent in
* the same string.
* If one command is not sent its value should be interpreted as it would not
* influence the search.
*/
void FrontEnd::uciGo(const std::string& input)
{
	int i = 1;
	int wtime = -1, btime = -1, winc = -1, binc = -1, movestogo = -1, depth = -1, nodes = -1, mate = -1, movetime = -1;
	string cmd, searchlist;
	bool ponder=false, infinite=false;
	cmd = getWord(input, i++);
	while (cmd.length()>0)
	{
		if (cmd == "searchmoves")
		{
			cmd = getWord(input, i++);
			if (isMoveText(cmd))
			{
				searchlist += cmd;
				searchlist += ' ';
			}
			i--;
		}
		else if (cmd == "ponder")
		{
			ponder = true;
		}
		else if (cmd == "wtime")
		{
			wtime = atoi(getWord(input, i++).c_str());
		}
		else if (cmd == "btime")
		{
			btime = atoi(getWord(input, i++).c_str());
		}
		else if (cmd == "winc")
		{
			winc = atoi(getWord(input, i++).c_str());
		}
		else if (cmd == "binc")
		{
			binc = atoi(getWord(input, i++).c_str());
		}
		else if (cmd == "movestogo")
		{
			movestogo = atoi(getWord(input, i++).c_str());
		}
		else if (cmd == "depth")
		{
			depth = atoi(getWord(input, i++).c_str());
		}
		else if (cmd == "nodes")
		{
			nodes = atoi(getWord(input, i++).c_str());
		}
		else if (cmd == "mate")
		{
			mate = atoi(getWord(input, i++).c_str());
		}
		else if (cmd == "movetime")
		{
			movetime = atoi(getWord(input, i++).c_str());
		}
		else if (cmd == "infinite")
		{
			infinite = true;
		}
		cmd = getWord(input, i++);
	}

	engine.send(ENG_position, currentBoard);
	if (ponder)
		engine.send(ENG_ponder);
	else
		engine.send(ENG_go);
}

/* stop
*
* stop calculating as soon as possible,
* don't forget the "bestmove" and possibly the "ponder" token when finishing
* the search
*/
void FrontEnd::uciStop()
{
	engine.send(ENG_stop);
}

/* ponderhit (UCI)
*
* the user has played the expected move. This will be sent if the engine was told
* to ponder on the same move the user has played. The engine should continue
* searching but switch from pondering to normal search.
*/
void FrontEnd::uciPonderhit()
{

}

void FrontEnd::uciMovegen(const std::string& s)
{
	DWORD i;
	char sz[256];
	i = movegenTest(atoi(getWord(s, 1).c_str()));
	sprintf_s(sz,256, "%u nodes", i);
	uci.write(string(sz));
}

void FrontEnd::engineInput()
{
}

DWORD FrontEnd::movegenTest(int depth, bool init, int ply)
{
	int moveit;
	static DWORD testNodes;
	static MoveList testList[30];
	static MoveGenerator testGen;
	static ChessBoard b;
	if (init)
	{
		testNodes=0;
		b.copy(currentBoard);
	}
	if (depth==0)
		return ++testNodes;
	testGen.makeMoves(b,testList[ply]);
	moveit=0;
	while (moveit!=testList[ply].end())
	{
		testGen.doMove(b,testList[ply].list[moveit]);
		movegenTest(depth-1,false,ply+1);
		testGen.undoMove(b,testList[ply].list[moveit]);
		++moveit;
	};
	return testNodes;
}

bool FrontEnd::isMoveText(const std::string& input)
{
	//  e2e4, e2-e4, e2xe4
	//  e4, ed, exd, exd4
	//  Ne4,Nxe4, N3e4, N3xe4, Nce4, Ncxe4
	//  O-O, o-o, 0-0, 
	if (input.length()<2) return false;
	if (strchr("NBRQKabcdefghoO0", input.at(0)) == NULL) return false;
	if (strchr("abcdefgh12345678-xX", input.at(1)) == NULL) return false;
	if (input.length()>2)
		if (strchr("abcdefgh12345678-xX!?#+", input.at(1)) == NULL) return false;
	if (input.length()>3)
		if (strchr("abcdefgh12345678-xX!?#+", input.at(1)) == NULL) return false;
	if (input.length()>4)
		if (strchr("abcdefgh12345678-xX!?#+", input.at(1)) == NULL) return false;
	return true;
};
