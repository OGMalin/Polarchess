#include <process.h>
#include "Engine.h"
#include "..\Common\Utility.h"

// Use with logging
//#include <fstream>
//std::ofstream logfileE;
//#define LOG(txt) logfileE.open("MoveDelay.log", ios::out | ios::app);logfileE << txt << endl;logfileE.close();
// Else
#define LOG(txt) {};

using namespace std;

CRITICAL_SECTION engineCS;

void Engine::engineInputThread(void* lpv)
{
	char   c;
	Engine* eng = (Engine*)lpv;
	string line = "";
	DWORD dwRead;
	DWORD dwError;
	while (!eng->abort)
	{
		if (eng->hRead == NULL)
			break;
		if (!ReadFile(eng->hRead, &c, 1, &dwRead, NULL))
		{
			dwError = GetLastError();
			if ((dwError == ERROR_BROKEN_PIPE) || (dwError == ERROR_HANDLE_EOF))
				break;
		}
		if (dwRead)
		{
			if (c == '\n')
			{
				eng->input(line);
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

Engine::Engine()
{
	InitializeCriticalSection(&engineCS);
	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	pi.hProcess = NULL;
	pi.hThread = NULL;
	hWrite = NULL;
	hRead = NULL;
	hThread = NULL;
}

Engine::~Engine()
{
	DeleteCriticalSection(&engineCS);
	CloseHandle(hEvent);

}

void Engine::write(const std::string& s)
{
	DWORD dw;
	if (hWrite)
	{
		LOG("E< "+s);
		WriteFile(hWrite, s.c_str(), (DWORD)s.length(), &dw, NULL);
		WriteFile(hWrite, "\n", 1, &dw, NULL);
	}
}

bool Engine::start(const std::string& path)
{
	if (path.empty())
		return false;

	// Save old handles
	HANDLE hOldOut, hOldIn, hOldErr;
	hOldOut = GetStdHandle(STD_OUTPUT_HANDLE);
	hOldIn = GetStdHandle(STD_INPUT_HANDLE);
	hOldErr = GetStdHandle(STD_ERROR_HANDLE);

	// Sets up the security to be the same as its parent (this) process
	// for the io handles
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	// ******************************************************************
	// First make a handle so we can read output from external program
	// ******************************************************************

	// Create anonymous pipe to be STDOUT for child process.
	// hReadOut is the handle to read messages from external program
	// hWriteOut is the handle the external program is using as a output handle.
	HANDLE hReadOut, hWriteOut;
	if (!CreatePipe(&hReadOut, &hWriteOut, &sa, 0))
		return 0;
	
	// Set STDOUT of the parent process to be write handle of
	// the pipe, so it is inherited by the child process.
	if (!SetStdHandle(STD_OUTPUT_HANDLE, hWriteOut))
		return 0;

	// Create a noninheritable duplicate of the read handle and
	// close the inheritable read handle.
	if (!DuplicateHandle(GetCurrentProcess(), hReadOut, GetCurrentProcess(), &hRead, 0, FALSE, DUPLICATE_SAME_ACCESS))
		return 0;
	CloseHandle(hReadOut);

	// ******************************************************************
	// Now it is time to make a handle in the opposite direction so that
	// we can write command to the external program
	// ******************************************************************

	//  Create anonymous pipe to be STDIN for external process.
	HANDLE hReadIn, hWriteIn;
	if (!CreatePipe(&hReadIn, &hWriteIn, &sa, 0))
		return 0;

	// Set STDIN of the parent to be the read handle of the
	// pipe, so it is inherited by the child process.
	if (!SetStdHandle(STD_INPUT_HANDLE, hReadIn))
		return 0;

	// Create a noninheritable duplicate of the write handle,
	// and close the inheritable write handle.
	if (!DuplicateHandle(GetCurrentProcess(), hWriteIn, GetCurrentProcess(), &hWrite, 0, FALSE, DUPLICATE_SAME_ACCESS))
		return 0;
	CloseHandle(hWriteIn);

	//*******************************************
	// Starting the extern application
	//*******************************************

	// si is where you tell the external program how to be showed. Since
	// this should be running in background only the new io handles is used.
	STARTUPINFO         si;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdInput = hReadIn;
	si.hStdOutput = hWriteOut;
	si.hStdError = hWriteOut;

	// Clear the process information
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

	char szPath[MAX_PATH];
	strcpy_s(szPath, MAX_PATH,path.c_str());
	if (!CreateProcess(NULL,
		szPath,
		NULL,
		NULL,
		TRUE,
		CREATE_NEW_PROCESS_GROUP | GetPriorityClass(GetCurrentProcess()) | DETACHED_PROCESS,
		NULL,
		NULL,
		&si,
		&pi))
	{
		return false;
	}

	// Adjust the thread priority
	SetThreadPriority(pi.hThread,GetThreadPriority(GetCurrentThread()));

	// After process creation, restore the saved STDIN and STDOUT.
	SetStdHandle(STD_INPUT_HANDLE, hOldIn);
	SetStdHandle(STD_OUTPUT_HANDLE, hOldOut);
	SetStdHandle(STD_ERROR_HANDLE, hOldErr);

	// The external program has got his copy of these so we don't need them
	// anymore.
	CloseHandle(hReadIn);
	CloseHandle(hWriteOut);


	// Start a thread to listen for input from the engine.
	abort = false;

	hThread = (HANDLE)_beginthread(Engine::engineInputThread, 0, this);
	if (hThread == (HANDLE)-1)
	{
		hThread = NULL;
		stop();
		return false;
	}
	return true;
}

void Engine::stop()
{
	DWORD dw;
	abort = true;
	if (pi.hProcess)
	{
		if (WaitForSingleObject(pi.hProcess, 3000) == WAIT_TIMEOUT)
			TerminateProcess(pi.hProcess, 0);
	}

	if (hWrite && GetHandleInformation(hWrite, &dw))
		CloseHandle(hWrite);
	if (hRead && GetHandleInformation(hRead, &dw))
		CloseHandle(hRead);
	if (pi.hProcess && GetHandleInformation(pi.hProcess, &dw))
		CloseHandle(pi.hProcess);
	if (pi.hThread && GetHandleInformation(pi.hThread, &dw))
		CloseHandle(pi.hThread);

	pi.hThread = NULL;
	pi.hProcess = NULL;
	hWrite = NULL;
	hRead = NULL;

	if (hThread)
	{
		if (WaitForSingleObject(hThread, 1000) == WAIT_TIMEOUT)
			TerminateThread(hThread, 0);
		hThread = NULL;
	}
}

void Engine::input(const std::string& s)
{
	if (s.length() == 0)
		return;
	EnterCriticalSection(&engineCS);
	inQue.push_back(s);
	LOG("E> " + s);
	LeaveCriticalSection(&engineCS);
	SetEvent(hEvent);
}

int Engine::get(std::string& s)
{
	int ret = ENGINE_none;
	EnterCriticalSection(&engineCS);
	if (inQue.size() > 0)
	{
		s = inQue.front();
		inQue.pop_front();
		ret = extract(s);
	}
	LeaveCriticalSection(&engineCS);
	return ret;
}

int Engine::extract(std::string& s)
{

	int ret = ENGINE_other;
	size_t len;
	string cmd;
	cmd = getWord(s, 1);
	if (cmd == "uciok")
		ret = ENGINE_uciok;
	else if (cmd == "info")
		ret = ENGINE_info;
	else if (cmd == "bestmove")
		ret = ENGINE_bestmove;
	if (ret != ENGINE_other)
	{
		len = cmd.length();
		s.erase(0, len);
	}
	s = trim(s);
	return ret;
}
