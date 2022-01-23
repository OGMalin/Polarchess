#include <process.h>
#include <iostream>
#include "DGT.h"
#include "../Common/DgtProtocol.h"
#include "../Common/MoveList.h"

HANDLE dgtHComm;
HANDLE dgtTimer;

CRITICAL_SECTION dgtCS;

using namespace std;

void dgtStopTimer()
{
	if (!dgtTimer)
		return;
	DeleteTimerQueueTimer(NULL, dgtTimer, NULL);
	dgtTimer = NULL;
}

VOID CALLBACK dgtTimerProc(VOID*, BOOLEAN)
{
	dgtStopTimer();
	TransmitCommChar(dgtHComm, DGT_REQ_BOARD);
}

void dgtStartTimer(int ms)
{
	if (dgtTimer)
		dgtStopTimer();
	CreateTimerQueueTimer(&dgtTimer, NULL, dgtTimerProc, NULL, ms, 0, WT_EXECUTEONLYONCE);
//	if (!CreateTimerQueueTimer(&dgtTimer, NULL, dgtTimerProc, NULL, ms, 0, WT_EXECUTEONLYONCE))
//		qDebug("Failed to create timer.");
}

void DGT::threadLoop(void*lpv)
{
	// Maks lengde på meldinger fra DGT brettet er 16k
	BYTE  buffer[0x4000];
	int   index;
	int   msgLen;
	BYTE  b;
	DWORD dwRead;
	DWORD dwError = 0;
	DGT* dgt = (DGT*)lpv;
	DWORD len = 0;
	index = 0;
	msgLen = 0x4000;
	while (!dgt->abort)
	{
		if (!ReadFile(dgtHComm, &b, 1, &dwRead, NULL))
		{
			dwError = GetLastError();
			if (dwError == ERROR_HANDLE_EOF)
				break;
		}
		// Timeout
		if (!dwRead)
		{
			//			dgt->checkForMove();
			continue;
		}

		// Check for wrong message.
		if (index == 0)
		{
//			qDebug("DGT -> Code: %x", b);
			if ((b & 0x80) == 0)
				continue;
		}

		buffer[index] = b;

		// Find messagesize
		if (index == 2)
		{
			msgLen = MSG_LENGTH(buffer[1], buffer[2]);
			if (msgLen > 0x4000)
				msgLen = 0x4000;
//			qDebug("DGT -> Msg length: %i", msgLen);
		}

		index++;
		if (index >= msgLen)
		{
//			qDebug(QString("DGT -> " + convertToHex(&buffer[3], msgLen - 3)).toLatin1());
			dgt->interpretMessage(buffer[0], msgLen - 3, &buffer[3]);
			index = 0;
			msgLen = 0x4000;
		}
	}
	dgtStopTimer(); // Just in case
	_endthread();
};

DGT::DGT()
{
	InitializeCriticalSection(&dgtCS);
	hThread = NULL;
	dgtHComm = NULL;
	abort = true;
	DgtBoardVersion = 0;
	DgtClockVersion = 0;
	clockConnected = false;
	waitForClockAck = false;
	dgtSetting.autoRotate = true;
	dgtSetting.port = "com4";
	dgtSetting.delay = 300;
	memset(guiBoard, 0, 64);
	memset(stableBoard, 0, 64);
	dgtTimer = NULL;
	rotated = false;
	// Create the event to tell the frontend that there is some messagefrom the board waiting
	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}

DGT::~DGT()
{
	DeleteCriticalSection(&dgtCS);
	CloseHandle(hEvent);
}

void DGT::interpretMessage(BYTE code, int datalength, BYTE* data)
{
	MoveList* ml = new MoveList;
	ChessBoard bS;
	ChessBoard bL;
	ChessMove move;
	//	DGT_MSG_VERSION_STRUCT version;
//	int i, j;
	string s;
	switch (code)
	{
	case DGT_MSG_BOARD_DUMP:
		if (datalength != 64)
		{
			
			cout << "DGT_MSG_BOARD_DUMP messagelength mismatch\n";
			break;
		}
		memcpy(stableBoard, data, 64);
		if (rotated)
			rotateBoard(stableBoard);
//		updateDialog(stableBoard);
		if (equalBoard(guiBoard, stableBoard))
			_status = 2;
		else
			_status = 1;
//		emit dgtStatus(_status);
		move = getLegalMove(stableBoard);
		if (move.empty())
		{
			// Is it something else?
			// New game
			ChessBoard cb;
			convertBoard(stableBoard, cb);
//			qDebug("Fen: %s", cb.getFen().c_str());

			if (cb.isStartposition())
			{
//				emit dgtNewGame();
				break;
			}
			//try a roteted board
			rotateBoard(stableBoard);
			convertBoard(stableBoard, cb);
//			qDebug("Fen: %s", cb.getFen().c_str());
			if (cb.isStartposition())
			{
				rotated = !rotated;
//				updateDialog(stableBoard);
//				emit dgtNewGame();
				break;
			}
			rotateBoard(stableBoard);

			// Result
			//if (cb.board[d4] == whiteking)
			//{
			//	if (cb.board[d5] == blackking)
			//		emit dgtResult(DRAW);
			//	else if (cb.board[e5] == blackking)
			//		emit dgtResult(BLACKWIN);
			//	break;
			//}
			//else if (cb.board[e4] == whiteking)
			//{
			//	if (cb.board[d5] == blackking)
			//		emit dgtResult(WHITEWIN);
			//	else if (cb.board[e5] == blackking)
			//		emit dgtResult(DRAW);
			//	break;
			//}
			// Mark the board with different between guiboard and stable board
			break;
		}
//		emit dgtNewMove(&move);
		break;
	case DGT_MSG_SBI_CLOCK:
		if (datalength != 7)
		{
//			qDebug("DGT_MSG_SBI_CLOCK messagelength mismatch");
			break;
		}
		if (((data[0] & 0x0f) == 0x0a) || ((data[3] & 0x0f) == 0x0a))
		{ // Clock ack message
			int ack0 = (data[1] & 0x7f) | ((data[3] << 3) & 0x80);
			int ack1 = (data[2] & 0x7f) | ((data[3] << 2) & 0x80);
			int ack2 = (data[4] & 0x7f) | ((data[0] << 3) & 0x80);
			int ack3 = (data[5] & 0x7f) | ((data[0] << 2) & 0x80);
			if (ack0 == 0x40)
			{
//				qDebug("Error: Sending CMD_CLOCK_SETNRUN while clock is not in mode 23/25");
//				writeClock();
				break;
			}
			//if (ack0 == 0x10)
			//	qDebug("Normal clock ack message");
			//else
			//	qDebug("Unknown ack message: %i", ack0);
			switch (ack1)
			{
//			case 0x81:qDebug("Clock ready ack"); break;
			//case 0x88:
			//	if (ack3 == 0x31)
			//		qDebug("Clock Back button pressed.");
			//	if (ack3 == 0x32)
			//		qDebug("Clock Plus button pressed.");
			//	if (ack3 == 0x33)
			//		qDebug("Clock Run button pressed.");
			//	if (ack3 == 0x34)
			//		qDebug("Clock Minus button pressed.");
			//	if (ack3 == 0x35)
			//		qDebug("Clock OK button pressed.");
			//	break;
			//case 0x8a:qDebug("Clock Mode: %i", ack3); break;
			//case 0x90:qDebug("Clock not in mode 23"); break;
			//case DGT_CMD_CLOCK_DISPLAY:qDebug("Display ack"); break;
			//case DGT_CMD_CLOCK_ICONS:qDebug("Icon ack"); break;
			//case DGT_CMD_CLOCK_END:qDebug("End ack"); break;
			//case DGT_CMD_CLOCK_BUTTON:qDebug("Button ack"); break;
			case DGT_CMD_CLOCK_VERSION:
				//clockVersion.major = ack2 >> 4;
				//clockVersion.minor = ack2 & 0x0f;
				DgtClockVersion = ack2 >> 4; //XL=1, 3000=2
				//qs.clear();
				//QTextStream(&qs) << (int)(ack2 >> 4) << "." << (((ack2 & 0x0f) < 10) ? "0" : "") << (int)(ack2 & 0x0f);
				//lClockVersion->setText(qs);
				//qDebug("Clock version: %s", qs.toLatin1());
				break;
			//case DGT_CMD_CLOCK_SETNRUN:qDebug("SetNRun ack"); break;
			//case DGT_CMD_CLOCK_BEEP:qDebug("Beep ack"); break;
			//case DGT_CMD_CLOCK_ASCII:qDebug("Ascii ack"); break;
			default:
//				qDebug("Command ack for: %i", ack1);
				break;
			}
			// Is this an acknowledge of a clock command? Then see if more command should be sendt.
			//if (ack1 & 0x80 != 0x80)
			//	writeClock();
		}
		else
		{ // Clock time message
//			memset(&clock.rightFlag, data[0], 1);
//			clock.rightMinutes = data[1];
//			clock.rightSeconds = data[2];
//			memset(&clock.leftFlag, data[3], 1);
//			clock.leftMinutes = data[4];
//			clock.leftSeconds = data[5];
//			memset(&clock.status, data[6], 1);
//			if (clock.status.noClock)
//			{
//				clockConnected = false;
//				qDebug("DGT No clock connected.");
//				lClockVersion->setText("No clock");
//			}
//			else if (!clockConnected)
//			{
//				clockConnected = true;
//				qDebug("DGT clock connected.");
//				// Get clock serial number
//				writeClock(DGT_CMD_CLOCK_VERSION);
//			}
//			//leftClock->setTime(QTime(clock.leftFlag.bcdHour, clock.leftMinutes, clock.leftSeconds));
//			//rightClock->setTime(QTime(clock.rightFlag.bcdHour, clock.rightMinutes, clock.rightSeconds));
////			if (clock.status.leftTurn)
//			{
//				qs.clear();
//				QTextStream(&qs) << clock.leftFlag.bcdHour << ":" << ((clock.leftMinutes & 0xf0) >> 4) << (clock.leftMinutes & 0x0f) << ":" << ((clock.leftSeconds & 0xf0) >> 4) << (clock.leftSeconds & 0x0f);
//				leftClock->setText(qs);
//				qDebug("Left clock: %s", qs.toLatin1());
//			}
//			//			if (clock.status.rightTurn)
//			{
//				qs.clear();
//				QTextStream(&qs) << clock.rightFlag.bcdHour << ":" << ((clock.rightMinutes & 0xf0) >> 4) << (clock.rightMinutes & 0x0f) << ":" << ((clock.rightSeconds & 0xf0) >> 4) << (clock.rightSeconds & 0x0f);
//				rightClock->setText(qs);
//				qDebug("Right clock: %s", qs.toLatin1());
//			}
//			//			QApplication::processEvents();
		}
		break;
	case DGT_MSG_FIELD_UPDATE:
		if (datalength != 2)
		{
//			qDebug("DGT_MSG_FIELD_UPDATE messagelength mismatch");
			break;
		}
		dgtStartTimer(dgtSetting.delay);
		//		dgtTimer = SetTimer(NULL, NULL, 500, staticStableTimeout);
				// Restart timer
		//		if (stableTimer)
		//			restartTimer = true;

				// Just for fun
				//field.field = data[0];
				//field.piece = data[1];
//		updateDialog(data[0], data[1]);
		break;
	case DGT_MSG_BUSADDRESS:
		if (datalength != 2)
		{
//			qDebug("DGT_MSG_BUSADDRESS messagelength mismatch");
			break;
		}
		//busadres = (data[0] & (BYTE)0x7f) << 7;
		//busadres += data[1] & (BYTE)0x7f;
		break;
	case DGT_MSG_SERIALNR:
		if (datalength != 5)
		{
//			qDebug("DGT_MSG_SERIALNR messagelength mismatch");
			break;
		}
		//data[datalength] = '\0';
		//serialnr = (char*)data;
		break;
	case DGT_MSG_TRADEMARK:
		//data[datalength] = '\0';
		//trademark = (char*)data;
		break;
	case DGT_MSG_VERSION:
		// Version are used to check for connection.
		if (datalength != 2)
		{
//			qDebug("DGT_MSG_VERSION messagelength mismatch");
			break;
		}
		switch (data[0])
		{
		case 0: DgtBoardVersion = 3; break; // Serial/USB.b
		case 1: DgtBoardVersion = 1; break; // Bluetooth
		case 3: DgtBoardVersion = 2; break; // Revetation II
		case 4: DgtBoardVersion = 4; break; // USB c
		}
		//boardVersion.major = data[0];
		//boardVersion.minor = data[1];
		//qs.clear();
		//QTextStream(&qs) << (int)data[0] << "." << ((data[1] < 10) ? "0" : "") << (int)data[1];
		//lBoardVersion->setText(qs);
		//statusLine->setText("Connected");
		//_status = 1;
		//emit dgtStatus(_status);
		//write(DGT_REQ_BOARD);
		//updateMode = DGT_REQ_UPDATE_SBI_NICE;
		//write(updateMode);
		//write(DGT_REQ_SBI_CLOCK);
		break;
	default:
		break;
	}
	delete ml;
	return;
}

bool DGT::equalBoard(BYTE*b1, BYTE*b2)
{
	for (int i = 0; i < 64; i++)
		if (b1[i] != b2[i])
			return false;
	return true;
}

bool DGT::equalBoard(ChessBoard& cb, BYTE* data)
{
	BYTE b[64];
	convertBoard(cb, b);
	return equalBoard(b, data);
}

// Convert from ChessBoard to DGT board
void DGT::convertBoard(ChessBoard& cb, BYTE* b)
{
	int piece[13] = { DGT_EMPTY,DGT_WPAWN,DGT_WKNIGHT,DGT_WBISHOP,DGT_WROOK,DGT_WQUEEN,DGT_WKING,DGT_BPAWN,DGT_BKNIGHT,DGT_BBISHOP,DGT_BROOK,DGT_BQUEEN,DGT_BKING };
	int i;
	int f, r;
	i = 0;
	for (r = 7; r > -1; r--)
	{
		for (f = 0; f < 8; f++)
		{
			b[i] = piece[cb.board[SQUARE(f, r)]];
			++i;
		}
	}
}

void DGT::convertBoard(BYTE* b, ChessBoard& cb)
{
	int piece[13] = { EMPTY,whitepawn,whiterook,whiteknight,whitebishop,whiteking,whitequeen,blackpawn,blackrook,blackknight,blackbishop,blackking,blackqueen };
	int i;
	int f, r;
	i = 0;
	for (r = 7; r > -1; r--)
	{
		for (f = 0; f < 8; f++)
		{
			cb.board[SQUARE(f, r)] = piece[b[i]];
			++i;
		}
	}
}

ChessMove DGT::getLegalMove(BYTE* data)
{
	MoveList ml;
	ChessBoard cb;
	BYTE b[64];
	int i;
	guiCBBoard.legalMoves(ml);
	for (i = 0; i < ml.size(); i++)
	{
		cb = guiCBBoard;
		cb.doMove(ml[i], false);
		convertBoard(cb, b);
		if (equalBoard(data, b))
			return ml[i];
	}
	return ChessMove();
}

void DGT::rotateBoard(BYTE* board)
{
	int i = 0, j = 63, k;
	while (i < j)
	{
		k = board[i];
		board[i] = board[j];
		board[j] = k;
		++i;
		--j;
	}
}

void DGT::connectDgt(string portname)
{
	DCB dcb;
	COMMTIMEOUTS timeout;
	DWORD Maske;
//	QSettings settings;
	disconnectDgt();
	//if (portname.isEmpty())
	//	portname = portlist->currentText();
	dgtHComm = CreateFileA(
		portname.c_str(),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,//FILE_FLAG_OVERLAPPED,
		NULL);
	if (dgtHComm == INVALID_HANDLE_VALUE)
	{
		dgtHComm = NULL;
		return;
	};

	dgtSetting.port = portname;

	// Set up communication parameters.
	dcb.DCBlength = sizeof(DCB);
	GetCommState(dgtHComm, &dcb);
	dcb.BaudRate = CBR_9600;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	dcb.fTXContinueOnXoff = 1;
	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.fBinary = 1;
	dcb.fParity = 0;
	dcb.fOutxCtsFlow = 0;
	dcb.fOutxDsrFlow = 0;
	dcb.fDsrSensitivity = 0;
	dcb.fOutX = 0;
	dcb.fInX = 0;
	dcb.fErrorChar = 0;
	dcb.fNull = 0;
	dcb.fRtsControl = RTS_CONTROL_DISABLE;
	dcb.fAbortOnError = 0;
	SetCommState(dgtHComm, &dcb);

	GetCommTimeouts(dgtHComm, &timeout);
	timeout.ReadIntervalTimeout = MAXDWORD;
	timeout.ReadTotalTimeoutMultiplier = MAXDWORD;
	timeout.ReadTotalTimeoutConstant = 500;
	timeout.WriteTotalTimeoutMultiplier = 0;
	timeout.WriteTotalTimeoutConstant = 0;

	SetCommTimeouts(dgtHComm, &timeout);
	if (!GetCommMask(dgtHComm, &Maske))
		return;
	Maske |= EV_RXCHAR;
	//	if (!SetCommMask(hComm,Maske))
	//   	return false;
	//	return true;

	  // Make an input thread
	abort = false;

	hThread = (HANDLE)_beginthread(DGT::threadLoop, 0, this);
	if (hThread == (HANDLE)-1)
	{
		hThread = NULL;
		abort = true;
		return;
	}

	// Give the DGT board some time to initilize (is it needed?).
	Sleep(1500);
	updateMode = DGT_REQ_RESET;
	write(updateMode);

	write(DGT_REQ_VERSION);
}

void DGT::disconnectDgt()
{
	dgtStopTimer();
	abort = true;
	if (hThread)
		if (WaitForSingleObject(hThread, 1000) == WAIT_TIMEOUT)
			TerminateThread(hThread, TRUE);
	if (dgtHComm != NULL)
		CloseHandle(dgtHComm);
	hThread = NULL;
	dgtHComm = NULL;
	_status = 0;
	//emit dgtStatus(_status);
	//statusLine->setText("Disconnected");
	//lBoardVersion->setText("No board");
	//lClockVersion->setText("No clock");
	updateMode = 0;
	clockConnected = false;
}
void DGT::writeClock(BYTE command, BYTE* message)
{
	//BYTE data[16];
	//if (command == 0)
	//{ // This is called when an clock ack is received.
	//	if (clockCommands.size() == 0)
	//	{
	//		waitForClockAck = false;
	//		return;
	//	}
	//	if ((updateMode == DGT_REQ_UPDATE_SBI) || (updateMode == DGT_REQ_UPDATE_SBI_NICE))
	//	{
	//		memcpy(data, clockCommands[0], 16);
	//		clockCommands.pop_front();
	//		waitForClockAck = true;
	//		write(data, data[1] + 2);
	//	}
	//	else
	//	{
	//		for (int i = 0; i < clockCommands.size(); i++)
	//		{
	//			memcpy(data, clockCommands[i], 16);
	//			write(data, data[1] + 2);
	//		}
	//		clockCommands.clear();
	//		waitForClockAck = false;
	//	}
	//	return;
	//}
	//data[0] = DGT_SBI_CLOCK_MESSAGE;
	//data[2] = DGT_CMD_CLOCK_START_MESSAGE;
	//data[3] = command;
	//switch (command)
	//{
	//case DGT_CMD_CLOCK_DISPLAY:
	//	data[1] = 0x0b; // Size
	//	memcpy(&data[4], message, 8);
	//	break;
	//case DGT_CMD_CLOCK_ICONS:
	//	data[1] = 0x0b; // Size
	//	memcpy(&data[4], message, 8);
	//	break;
	//case DGT_CMD_CLOCK_END:
	//	data[1] = 0x03; // Size
	//	break;
	//case DGT_CMD_CLOCK_BUTTON:
	//	data[1] = 0x03; // Size
	//	break;
	//case DGT_CMD_CLOCK_VERSION:
	//	data[1] = 0x03; // Size
	//	break;
	//case DGT_CMD_CLOCK_SETNRUN:
	//	data[1] = 0x0a; // Size
	//	memcpy(&data[4], message, 7);
	//	break;
	//case DGT_CMD_CLOCK_BEEP:
	//	data[1] = 0x04; // Size
	//	data[4] = message[0];
	//	break;
	//case DGT_CMD_CLOCK_ASCII:
	//	data[1] = 0x0c; // Size
	//	memcpy(&data[4], message, 9);
	//	break;
	//default:
	//	return;
	//}
	//data[data[1] + 1] = DGT_CMD_CLOCK_END_MESSAGE;
	//if (((updateMode == DGT_REQ_UPDATE_SBI) || (updateMode == DGT_REQ_UPDATE_SBI_NICE)) && waitForClockAck)
	//{
	//	if (waitForClockAck)
	//	{
	//		clockCommands.push_back(QByteArray((char*)data, 16));
	//	}
	//	else
	//	{
	//		waitForClockAck = true;
	//		write(data, data[1] + 2);
	//	}
	//}
	//else
	//{
	//	waitForClockAck = false;
	//	write(data, data[1] + 2);
	//}
}

void DGT::write(BYTE command)
{
	//	if (_status == 0)
	//		return;
	if (!dgtHComm)
	{
//		qDebug("DGT <- Error: Try to write when port is not open.");
		_status = 0;
		return;
	}
	TransmitCommChar(dgtHComm, command);
//	qDebug(QString(QString("DGT <- ") + convertToHex(&command, 1)).toLatin1());
}
void DGT::write(BYTE* message, int length)
{

	if (!dgtHComm)
	{
//		qDebug("DGT <- Error: Try to write when port is not open.");
		_status = 0;
		return;
	}
	DWORD written;
	WriteFile(dgtHComm, message, length, &written, NULL);
//	qDebug(QString(QString("DGT <- ") + convertToHex(message, length)).toLatin1());
	//if (written != length)
	//	qDebug("DGT -> Try to write %i bytes but did only write %i bytes.", length, written);
}
