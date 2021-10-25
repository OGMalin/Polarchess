#pragma once
#include <Windows.h>
#include <QDialog>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QSlider>
#include "../Common/ChessBoard.h"
#include "../Common/ChessMove.h"
#include "../Common/BoardWindow.h"
#include "../Common/DgtProtocol.h"

//#ifndef BYTE
//#define BYTE unsigned char
//#endif
enum { DRAW, WHITEWIN, BLACKWIN };

struct DgtSetting
{
	QString port;
	int delay;
	bool autoRotate;
};

//
// Interfacing with a Dgt electronic chess board.

class DgtBoard : public QDialog
{
	Q_OBJECT
public slots:
	void listPorts();
	void connectDgt(QString portname=QString());
	void disconnectDgt();
//	void dgtPortOpen(bool);
	void interpretMessage(BYTE code, int datalength, BYTE* data);
	void delaytimeChanged(int);
	void autorotateChanged(int);
	void rotate();
signals:
//	// Send last status.
//	// 0 = Not connected
//	// 1 = Connected but not in sync
//	// 2 = Connected and in sync
	void dgtStatus(int);
//	void newPosition(ChessBoard&, MoveList&);
	void dgtNewMove(ChessMove*);
	void dgtNewGame();
	void dgtResult(int);
public:
	DgtBoard(QWidget* parent = 0);
	~DgtBoard();
//	// Return values: 0=Not connected, 1=Connected out of sync, 2=Connected in sync
//	int status();
//	// Let the driver know the GUI position
	void setBoard(ChessBoard&);
	bool autoConnect();
	DgtSetting setting();
	void setting(DgtSetting&);
private:
	// 1=Serial/USB-b, 2=Bluetooth, 3=Revelation II, 4=USV-c
	int DgtBoardVersion;
	// 1=DT XL, 2=DGT 3000
	int DgtClockVersion;
	DgtSetting dgtSetting;
	bool clockConnected;
	bool waitForClockAck;
	BYTE updateMode;
	bool rotated;
	QList<QByteArray> clockCommands;
//	DGT_MSG_VERSION_STRUCT boardVersion;
//	DGT_MSG_VERSION_STRUCT clockVersion;
	ChessBoard guiCBBoard; // Gui board
	BYTE guiBoard[64]; // Gui board in dgt format
	BYTE stableBoard[64]; // Last stable board
////	BYTE lastBoard[64]; // Last received board from the DGT board
	DGT_MSG_SBI_CLOCK_STRUCT clock;
//	DGT_MSG_FIELD_UPDATE_STRUCT field;
//	QString trademark;
//	int busadres;
	int _status;
//	QString serialnr;
	QComboBox* portlist;
	QCheckBox* autorotate;
	QLabel* statusLine;
	QLabel* lBoardVersion;
	QLabel* lClockVersion;
////	QTimeEdit* leftClock;
////	QTimeEdit* rightClock;
	QLabel* leftClock;
	QLabel* rightClock;
	QSlider* timedelay;
	QLabel* timedelayValue;
	BoardWindow* boardwindow;
////	QTimer* stableTimer;
	void write(BYTE command);
	void write(BYTE* message, int length);
	void writeClock(BYTE command=0, BYTE* message=NULL);
	void updateDialog(BYTE*);
	void updateDialog(BYTE fiield, BYTE piece);
	bool equalBoard(BYTE*, BYTE*);
	bool equalBoard(ChessBoard&, BYTE*);
	void convertBoard(ChessBoard& from, BYTE* to);
	void convertBoard(BYTE* from, ChessBoard& to);
	void rotateBoard(BYTE* board);
//	// Search to find the moves between the two board
//	void findPossibleMoves(MoveList*, ChessBoard& start, ChessBoard& end);
	ChessMove getLegalMove(BYTE*);
	static void threadLoop(void* lpv);
public:
	bool abort;
	HANDLE hThread;
};
