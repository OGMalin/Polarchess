#include <process.h>
#include "GUI.h"
#include "..\Common\Utility.h"

#ifdef _DEBUG
#include <fstream>
std::ofstream logfileG;
#define LOG(txt) logfileG.open("logfile.log", ios::out | ios::app);logfileG << txt << endl;logfileG.close();
#endif

using namespace std;

CRITICAL_SECTION guiCS;

void GUI::guiInputThread(void* lpv)
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
	hThread = (HANDLE)_beginthread(GUI::guiInputThread, 0, this);
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
	string ss = s;
	EnterCriticalSection(&guiCS);
	translate(ss);
	inQue.push_back(ss);
#ifdef _DEBUG
	LOG("G > " + s);
#endif
	LeaveCriticalSection(&guiCS);
	SetEvent(hEvent);
	if (s == "quit")
		return false;
	return true;
}

void GUI::write(const std::string& s)
{
	DWORD dw;
#ifdef _DEBUG
	LOG("GUI<- " + s);
#endif
	WriteFile(hWrite, s.c_str(), (DWORD)s.length(), &dw, NULL);
	WriteFile(hWrite, "\n", 1, &dw, NULL);
}

int GUI::extract(std::string& s)
{

	int ret = GUI_other;
	size_t len;
	string cmd;
	cmd = getWord(s, 1);
	if (cmd == "quit")
		ret = GUI_quit;
	if (cmd == "setoption")
		ret = GUI_setoption;
	if (cmd == "go")
		ret = GUI_go;
	if (cmd == "position")
		ret = GUI_position;
	if (cmd == "test")
		ret = GUI_test;
	if (cmd == "stop")
		ret = GUI_stop;
	if (ret != GUI_other)
	{
		len = cmd.length();
		s.erase(0, len);
	}
	s = trim(s);
	return ret;
}

void GUI::readTranslation(string ini, char* sz)
{
	char value[256];
	size_t index = 0, st;
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
			translation.insert(map<string, string>::value_type(skey, svalue));
			index += skey.length() + 1;
		}
	}
}

void GUI::translate(std::string& input)
{
	size_t st;
	map<string, string>::iterator transIt;
	transIt = translation.begin();
	while (transIt != translation.end())
	{
		st = input.find((*transIt).first);
		while (st != string::npos)
		{
			input.replace(st, (*transIt).first.length(), (*transIt).second);
			st = input.find((*transIt).first, st + (*transIt).second.length());
		}
		++transIt;
	}
}