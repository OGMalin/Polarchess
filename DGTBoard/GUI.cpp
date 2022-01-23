#include <process.h>
#include "GUI.h"
#include "../Common/Utility.h"

using namespace std;

CRITICAL_SECTION guiCS;

void GUI::threadLoop(void* lpv)
{
	char   c;
	GUI* it = (GUI*)lpv;
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

GUI::GUI()
{
	InitializeCriticalSection(&guiCS);

	// Create the event to tell the frontend that a user command is waiting
	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	hRead = GetStdHandle(STD_INPUT_HANDLE);
	hWrite = GetStdHandle(STD_OUTPUT_HANDLE);

	// Start the thread to wait for user.
	hThread = (HANDLE)_beginthread(GUI::threadLoop, 0, this);
}

GUI::~GUI()
{
	DeleteCriticalSection(&guiCS);
	CloseHandle(hEvent);
}

int GUI::get(std::string& s)
{
	int ret = GUI_none;
	EnterCriticalSection(&guiCS);
	if (inQue.size() > 0)
	{
		s = inQue.front();
		inQue.pop_front();
		ret = extract(s);
	}
	LeaveCriticalSection(&guiCS);
	return ret;
}

bool GUI::input(const std::string& s)
{
	if (s.length() == 0)
		return true;
	EnterCriticalSection(&guiCS);
	inQue.push_back(s);
	LeaveCriticalSection(&guiCS);
	SetEvent(hEvent);
	if (s == "quit")
		return false;
	return true;
}

void GUI::write(const std::string& s)
{
	DWORD dw;
	WriteFile(hWrite, s.c_str(), (DWORD)s.length(), &dw, NULL);
	WriteFile(hWrite, "\n", 1, &dw, NULL);
}

int GUI::extract(std::string& s)
{

	int ret = GUI_unknown;
	size_t len;
	string cmd;
	cmd = getWord(s, 1);
	if (cmd == "quit")
		ret = GUI_quit;
	else if (cmd == "eboard")
		ret = GUI_eboard;
	else if (cmd == "connect")
		ret = GUI_connect;
	else if (cmd == "disconnect")
		ret = GUI_disconnect;
	else if (cmd == "debug")
		ret = GUI_debug;
	if (ret != GUI_unknown)
	{
		len = cmd.length();
		s.erase(0, len);
	}

	s = trim(s);
	return ret;
}
