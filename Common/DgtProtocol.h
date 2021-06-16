#pragma once

// Search the internet for dgtbrd13.h from DGT to find better description of these contants.

#include <windows.h>
//#ifndef BYTE
//#define BYTE unsigned char
//#endif // !BYTE


// Commands to the DGT board. This is sendt as a single byte command.
//
// Set the board in idle mode, cancel any update mode. No result message.
const BYTE DGT_SEND_RESET = 0x40;
// Ask for clock update. Result in DGT_MSG_BWTIME message.
const BYTE DGT_SEND_CLK = 0x41;
// Get the complete board. Result in DGT_MSG_BOARD_DUMP message.
const BYTE DGT_SEND_BRD = 0x42;
const BYTE DGT_SEND_UPDATE = 0x43; // Automatic send changes and clock.
const BYTE DGT_SEND_UPDATE_BRD = 0x44; // Automatic send changes on board.
const BYTE DGT_RETURN_SERIALNR = 0x45; // Get serial number.
const BYTE DGT_RETURN_BUSADRES = 0x46; // Get bus address.
const BYTE DGT_SEND_TRADEMARK = 0x47; // Get trademark message.
const BYTE DGT_SEND_EE_MOVES = 0x49; // Get stored moves.
const BYTE DGT_TO_BUSMODE = 0x4a; // Set board in busmode.
const BYTE DGT_SEND_UPDATE_NICE = 0x4b; // Automatic send changes and clock.
const BYTE DGT_SEND_BATTERY_STATUS = 0x4c; // Request battery status for bluetooth board
const BYTE DGT_SEND_VERSION = 0x4d; // Get version.
const BYTE DGT_RETURN_LONG_SERIALNR = 0x55; // Get long serial number from bluetooth board

// Message from the DGT board
const BYTE DGT_MSG_ERROR = 0x00;
const BYTE DGT_MSG_NONE = 0x80; //
const BYTE DGT_MSG_BOARD_DUMP = 0X86;
const BYTE DGT_MSG_BOARD_DUMP_SIZE = 64;
const BYTE DGT_MSG_BWTIME = 0x8d; //
const BYTE DGT_MSG_BWTIME_SIZE = 7; //
const BYTE DGT_MSG_FIELD_UPDATE = 0x8e; //
const BYTE DGT_MSG_FIELD_UPDATE_SIZE = 2;
const BYTE DGT_MSG_EE_MOVES = 0x8f; //
const BYTE DGT_MSG_BUSADRES = 0x90; //
const BYTE DGT_MSG_BUSADRES_SIZE = 2;
const BYTE DGT_MSG_SERIALNR = 0x91; //
const BYTE DGT_MSG_SERIALNR_SIZE = 5;
const BYTE DGT_MSG_TRADEMARK = 0x92; //
const BYTE DGT_MSG_VERSION = 0x93; //
const BYTE DGT_MSG_VERSION_SIZE = 2;
const BYTE DGT_MSG_BATTERY_STATUS = 0xb0; // Battery status for bluetooth board
const BYTE DGT_MSG_BATTERY_STATUS_SIZE = 4;
const BYTE DGT_MSG_LONG_SERIALNR = 0xb2; // Battery status for bluetooth board
const BYTE DGT_MSG_LONG_SERIALNR_SIZE = 10;

// Board dump message
// The board is scanned from upper left corner (connector on left side).
const BYTE DGT_EMPTY = 0x00;
const BYTE DGT_WPAWN = 0x01;
const BYTE DGT_WROOK = 0x02;
const BYTE DGT_WKNIGHT = 0x03;
const BYTE DGT_WBISHOP = 0x04;
const BYTE DGT_WKING = 0x05;
const BYTE DGT_WQUEEN = 0x06;
const BYTE DGT_BPAWN = 0x07;
const BYTE DGT_BROOK = 0x08;
const BYTE DGT_BKNIGHT = 0x09;
const BYTE DGT_BBISHOP = 0x0a;
const BYTE DGT_BKING = 0x0b;
const BYTE DGT_BQUEEN = 0x0c;
const BYTE DGT_PIECE1 = 0x0d;
const BYTE DGT_PIECE2 = 0x0e;
const BYTE DGT_PIECE3 = 0x0f;

struct DGT_MSG_VERSION_STRUCT
{
	BYTE major;
	BYTE minor;
};

struct DGT_MSG_BWTIME_FLAG
{
	unsigned int bcdHour : 4;
	unsigned int flagBlock : 1;
	unsigned int timePerMove : 1;
	unsigned int flag : 1;
	unsigned int unused : 1;
};

struct DGT_MSG_BWTIME_STATUS
{
	unsigned int running : 1;
	unsigned int tumbleLWhite : 1;
	unsigned int batteryLow : 1;
	unsigned int blackTurn : 1;
	unsigned int whiteTurn : 1;
	unsigned int noClock : 1;
	unsigned int unused : 2;
};

struct DGT_MSG_BWTIME_STRUCT
{
	DGT_MSG_BWTIME_FLAG leftFlag;
	BYTE leftMinutes;
	BYTE leftSeconds;
	DGT_MSG_BWTIME_FLAG rightFlag;
	BYTE rightMinutes;
	BYTE rightSeconds;
	DGT_MSG_BWTIME_STATUS status;
};

struct DGT_MSG_FIELD_UPDATE_STRUCT
{
	BYTE field;
	BYTE piece;
};
