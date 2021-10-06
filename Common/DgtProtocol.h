#pragma once

// These constants are from Ref:72002 e-Board protocol description version P1.0

// This is not all constants but what I use for single board communication

//#include <windows.h>
#ifndef BYTE
#define BYTE unsigned char
#endif // !BYTE

// Single board command codes from computer to board without data
const BYTE DGT_REQ_RESET = 0x40; // Set board in idle mode, cancel any update mode. No result message.
const BYTE DGT_REQ_SBI_CLOCK = 0x41; // Ask for clock data. Result in DGT_MSG_SBI_CLOCK message.
const BYTE DGT_REQ_BOARD = 0x42; // Get the complete board. Result in DGT_MSG_BOARD_DUMP message.
const BYTE DGT_REQ_UPDATE_SBI = 0x43; // Automatic send changes and clock every fixed period. Result in DGT_MSG_FIELD_UPDATE and DGT_MSG_SBI_CLOCK message.
const BYTE DGT_REQ_UPDATE_BOARD = 0x44; // Automatic send changes on board every fied period.  Result in DGT_MSG_FIELD_UPDATE message.
const BYTE DGT_REQ_SERIALNR = 0x45; // Get serial number. Result in DGT_MSG_SERIALNR message.
const BYTE DGT_REQ_BUSADDRESS = 0x46; // Get bus address. Result in DGT_MSG_BUSADDRESS message.
const BYTE DGT_REQ_TRADEMARK = 0x47; // Get trademark message.  Result in DGT_MSG_TRADEMARK message.
const BYTE SGT_REQ_HARDWARE_VERSION = 0x48; // Reqest for the hardware version of the board. Result in a DGT_MSG_HARDWARE_VERSION message.
const BYTE DGT_REQ_LOG_MOVES = 0x49; // Get stored moves not requested before. Result in DGT_MSG_LOG_MOVES message.
const BYTE DGT_TO_BUSMODE = 0x4a; // Set board in busmode. No result message.
const BYTE DGT_REQ_UPDATE_SBI_NICE = 0x4b; // Automatic send changes and clock. Result in DGT_MSG_FIELD_UPDATE and DGT_MSG_SBI_CLOCK message.
const BYTE DGT_REQ_BATTERY_STATUS = 0x4c; // Request battery status for board with battery. Result in DGT_MSG_BATTERY_STATUS message.
const BYTE DGT_REQ_VERSION = 0x4d; // Get version. Result in DGT_MSG_VERSION message.
//const BYTE DGT_STARTBOOTLOADER = 0X4e; // No result message.
const BYTE DGT_REQ_LONG_SERIALNR = 0x55; // Get long serial number from bluetooth board. Result in DGT_LONG_SERIALNR message.
const BYTE DGT_REQ_I2C_CLOCK = 0x56; // Request for clock data for clocks with I2C communication. Result in DGT_MSG_I2C_CLOCK message.
const BYTE DGT_REQ_UPDATE_I2C = 0x57; // Request for board and clock information after changes. Result in DGT_MSG_FIELD_UPDATE and DGT_MSG_I2C_CLOCK message.
const BYTE DGT_REQ_UPDATE_I2C_NICE = 0x58; // Request for board and clock information every fixed period. Result in DGT_MSG_FIELD_UPDATE and DGT_MSG_I2C_CLOCK message.

// Single board command codes from computer to board containing data
const BYTE DGT_SBI_CLOCK_MESSAGE = 0X2B; // Send command to the clock. Result in DGT_MSG_ACK message if board in UPDATE or UPDATE_NICE mode, else no result message.
// Format: DGT_SBI_CLOCK_MESSAGE:<Length>:DGT_CMD_CLOCK_START_MESSAGE:<command>:<content>:DGT_CMD_CLOCK_END_MESSAGE
const BYTE DGT_I2C_CLOCK_MESSAGE = 0x2c;
const BYTE DGT_SET_LEDS = 0x60; // Set LED pattern on Revelation II.

// Message from the DGT board
//const BYTE DGT_MSG_ERROR = 0x00;
//const BYTE DGT_MSG_NONE = 0x80; //
const BYTE DGT_MSG_BOARD_DUMP = 0X86;
//const BYTE DGT_MSG_BOARD_DUMP_SIZE = 64;
const BYTE DGT_MSG_SBI_CLOCK = 0x8d; //
//const BYTE DGT_MSG_BWTIME_SIZE = 7; //
const BYTE DGT_MSG_FIELD_UPDATE = 0x8e; //
//const BYTE DGT_MSG_FIELD_UPDATE_SIZE = 2;
const BYTE DGT_MSG_LOG_MOVES = 0x8f; //
const BYTE DGT_MSG_BUSADDRESS = 0x90; //
//const BYTE DGT_MSG_BUSADRES_SIZE = 2;
const BYTE DGT_MSG_SERIALNR = 0x91; //
//const BYTE DGT_MSG_SERIALNR_SIZE = 5;
const BYTE DGT_MSG_TRADEMARK = 0x92; //
const BYTE DGT_MSG_VERSION = 0x93; //
//const BYTE DGT_MSG_VERSION_SIZE = 2;
const BYTE DGT_MSG_HARDWARE_VERSION = 0x096;
const BYTE DGT_MSG_BATTERY_STATUS = 0xa0; // Battery status for bluetooth board
//const BYTE DGT_MSG_BATTERY_STATUS_SIZE = 4;
const BYTE DGT_MSG_LONG_SERIALNR = 0xa2; // Long serial number for bluetooth board
//const BYTE DGT_MSG_LONG_SERIALNR_SIZE = 10;
const BYTE DGT_MSG_I2C_CLOCK = 0Xa3;
#define MSG_LENGTH(b0,b1) ((((BYTE)b0 & 0x7f)<<7) + ((BYTE)b1&0x7f))

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
const BYTE DGT_PIECE1 = 0x0d; // Draw
const BYTE DGT_PIECE2 = 0x0e; // White wins
const BYTE DGT_PIECE3 = 0x0f; // Black wins

// Clock commands
const BYTE DGT_CMD_CLOCK_DISPLAY = 0x01; // Show a message or move on the clock using dots.
const BYTE DGT_CMD_CLOCK_ICONS = 0x02; // Shows div. flags on the clock.
const BYTE DGT_CMD_CLOCK_END = 0x03; // Clear the message on the clock and return back to showing clock time.
const BYTE DGT_CMD_CLOCK_BUTTON = 0x08; // Request the current button pressed.
const BYTE DGT_CMD_CLOCK_VERSION = 0x09; // Request clock version
const BYTE DGT_CMD_CLOCK_SETNRUN = 0x0a; // Set the clock in mode 23 and start counting down or pause the clock.
const BYTE DGT_CMD_CLOCK_BEEP = 0x0b; // Beep for a specified time.
const BYTE DGT_CMD_CLOCK_ASCII = 0x0c; // Send a message in ascii to the clock. Only for DGT3000
const BYTE DGT_CMD_CLOCK_START_MESSAGE = 0x03;
const BYTE DGT_CMD_CLOCK_END_MESSAGE = 0x00;


struct DGT_MSG_VERSION_STRUCT
{
	BYTE major;
	BYTE minor;
};

struct DGT_MSG_SBI_CLOCK_FLAG
{
	unsigned int bcdHour : 4; // 0-9
	unsigned int flagFallen : 1; // Flag fallen and clock blocked to zero.
	unsigned int timePerMove : 1; // Bronstein or Fischer timecontrol
	unsigned int flag : 1; // Flag fallen and indicated on display, clock could still be running.
	unsigned int unused : 1;
};

struct DGT_MSG_SBI_CLOCK_STATUS
{
	unsigned int running : 1;
	unsigned int leverRight : 1;
	unsigned int batteryLow : 1;
	unsigned int leftTurn : 1; // Check if left and right are switched since this is opposite of the rest of the protocol.
	unsigned int rightTurn : 1;
	unsigned int noClock : 1;
	unsigned int unused : 2; // Bit 7 allways 0
};

struct DGT_MSG_SBI_CLOCK_STRUCT
{
	DGT_MSG_SBI_CLOCK_FLAG rightFlag;
	BYTE rightMinutes;
	BYTE rightSeconds;
	DGT_MSG_SBI_CLOCK_FLAG leftFlag;
	BYTE leftMinutes;
	BYTE leftSeconds;
	DGT_MSG_SBI_CLOCK_STATUS status;
};

struct DGT_MSG_FIELD_UPDATE_STRUCT
{
	BYTE field;
	BYTE piece;
};

/*
A=0x77
B=0x7f
C=0x39
D=0x5e (d)
E=0x79
F=0x71
G=0x3d
H=0x76
I=0x06
J=0x1e
K=0x76 (H)
L=0x38
M=0x33 (?)
N=0x37 (n)
O=0x3f
P=0x73
Q=0x67 (q)
R=0x31
S=0x6d
T=0x78 (t)
U=0x3e
V=0x3e (U)
W=0x3c (?)
X=0x76 (H)
Y=0x6e
Z=0x5b
a=0x77 (A)
b=0x7c
c=0x58
d=0x5e
e=0x79 (E)
f=0x71 (F)
g=0x3d (G)
h=0x74
i=0x04
j=0x0c
k=0x76 (H)
l=0x30
m=0x33 (?)
n=0x54
o=0x5c
p=0x73 (P)
q=0x67
r=0x50
s=0x4c
t=0x78
u=0x1c
v=0x1c (u)
w=0x3c (?)
x=0x76 (H)
y=0x6e (Y)
z=0x5b (Z)
0=0x3f
1=0x06
2=0x5b
3=0x4f
4=0x33
5=0x6d
6=0x7d
7=0x07
8=0x7f
9=0x6f
*/