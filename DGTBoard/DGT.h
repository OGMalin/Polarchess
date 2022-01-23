#pragma once
#include <windows.h>
#include <string>
#include "../Common/ChessBoard.h"

enum { DRAW, WHITEWIN, BLACKWIN };

struct DgtSetting
{
	std::string port;
	int delay;
	bool autoRotate;
};

class DGT
{
public:
	// 1=Serial/USB-b, 2=Bluetooth, 3=Revelation II, 4=USV-c
	int DgtBoardVersion;
	// 1=DT XL, 2=DGT 3000
	int DgtClockVersion;
	bool clockConnected;
	bool waitForClockAck;
	BYTE updateMode;
	bool rotated;
	int _status;
	HANDLE hEvent;
	HANDLE hThread;
	bool abort;
	DgtSetting dgtSetting;
	ChessBoard guiCBBoard; // Gui board
	BYTE guiBoard[64]; // Gui board in dgt format
	BYTE stableBoard[64]; // Last stable board
	DGT();
	virtual ~DGT();
	static void threadLoop(void* lpv);
	void interpretMessage(BYTE code, int datalength, BYTE* data);
	bool equalBoard(BYTE*, BYTE*);
	bool equalBoard(ChessBoard&, BYTE*);
	void convertBoard(ChessBoard& from, BYTE* to);
	void convertBoard(BYTE* from, ChessBoard& to);
	void rotateBoard(BYTE* board);
	//	// Search to find the moves between the two board
	//	void findPossibleMoves(MoveList*, ChessBoard& start, ChessBoard& end);
	ChessMove getLegalMove(BYTE*);
	void write(BYTE command);
	void write(BYTE* message, int length);
	void writeClock(BYTE command = 0, BYTE* message = NULL);
	void connectDgt(std::string portname = "");
	void disconnectDgt();

};