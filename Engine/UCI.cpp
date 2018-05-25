#include <windows.h>
#include <process.h>
#include <string>

#include "UCI.h"
#include "../Common/Utility.h"

using namespace std;

CRITICAL_SECTION uciCS;

void UCI::threadLoop(void* lpv)
{
	char   c;
	UCI* it = (UCI*)lpv;
	string line = "";
	DWORD dwRead;
	DWORD dwError;
	while (1)
	{
		if (it->hRead == NULL)
			break;
		if (!ReadFile(it->hRead, &c, 1, &dwRead, NULL))
		{
			dwError = GetLastError();
			if ((dwError == ERROR_BROKEN_PIPE) || (dwError == ERROR_HANDLE_EOF))
				break;
		}
		if (dwRead)
		{
			if (c == '\n')
			{
				if (!it->input(line))
					break;
				line = "";
			}
			else if (c != '\r')
			{
				line += c;
			}
		}
	};
	_endthread();
}

UCI::UCI()
{
	InitializeCriticalSection(&uciCS);

	// Create the event to tell the frontend that a user command is waiting
	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	hRead = GetStdHandle(STD_INPUT_HANDLE);
	hWrite = GetStdHandle(STD_OUTPUT_HANDLE);

	// Start the thread to wait for user.
	hThread = (HANDLE)_beginthread(UCI::threadLoop, 0, this);
}

UCI::~UCI()
{
	DeleteCriticalSection(&uciCS);
	CloseHandle(hEvent);
}

int UCI::get(std::string& s)
{
	int ret = UCI_none;
	EnterCriticalSection(&uciCS);
	if (inQue.size() > 0)
	{
		s = inQue.front();
		inQue.pop_front();
		ret = extract(s);
	}
	LeaveCriticalSection(&uciCS);
	return ret;
}

bool UCI::input(const std::string& s)
{
	if (s.length() == 0)
		return true;
	EnterCriticalSection(&uciCS);
	inQue.push_back(s);
	LeaveCriticalSection(&uciCS);
	SetEvent(hEvent);
	if (s == "quit")
		return false;
	return true;
}

void UCI::write(const std::string& s)
{
	DWORD dw;
	WriteFile(hWrite, s.c_str(), (DWORD)s.length(), &dw, NULL);
	WriteFile(hWrite, "\n", 1, &dw, NULL);
}

int UCI::extract(std::string& s)
{

	int ret = UCI_unknown;
	size_t len;
	string cmd;
	cmd=getWord(s, 1);
	if (cmd == "debug")
		ret = UCI_debug;
	else if (cmd == "go")
		ret = UCI_go;
	else if (cmd == "isready")
		ret = UCI_isready;
	else if (cmd == "ponderhit")
		ret = UCI_ponderhit;
	else if (cmd == "position")
		ret = UCI_position;
	else if (cmd == "quit")
		ret = UCI_quit;
	else if (cmd == "register")
		ret = UCI_register;
	else if (cmd == "stop")
		ret = UCI_stop;
	else if (cmd == "uci")
		ret = UCI_uci;
	else if (cmd == "setoption")
		ret = UCI_setoption;
	else if (cmd == "ucinewgame")
		ret = UCI_ucinewgame;
	else if (cmd == "movegen")
		ret = UCI_movegen;
	else if (cmd == "readfile")
		ret = UCI_readfile;
	else if (cmd == "eval")
		ret = UCI_eval;
	if (ret != UCI_unknown)
	{
		len = cmd.length();
		s.erase(0, len);
	}

	s = trim(s);
	return ret;

}
