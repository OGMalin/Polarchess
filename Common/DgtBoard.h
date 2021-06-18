#pragma once
#include <QDialog>
#include <QWidget>
#include <QComboBox>
#include <QSerialPort>
#include <QLabel>
#include <QByteArray>
#include <QTimer>
#include "../Common/ChessBoard.h"
#include "../Common/BoardWindow.h"
#include "../Common/MoveList.h"
#include "../Common/DgtProtocol.h"

// Interfacing with a Dgt electronic chess board.
class DgtBoard : public QDialog
{
	Q_OBJECT
public slots:
	void listPorts();
	void connectDgt();
	void disconnectDgt();
//	void readFromDGT();
	void readyRead();
	void timerMessage();
	void comError(QSerialPort::SerialPortError);
signals:
	// Send last status.
	// 0 = Not connected
	// 1 = Connected but not in sync
	// 2 = Connected and in sync
	void dgtStatus(int);
	void newPosition(ChessBoard&, MoveList&);
	void newMove(ChessMove&);
public:
	DgtBoard(QWidget* parent = 0);
	~DgtBoard();
	// Return values: 0=Not connected, 1=Connected out of sync, 2=Connected in sync
	int status();
	// Let the driver know the GUI position
	void setBoard(ChessBoard&);
private:
	DGT_MSG_VERSION_STRUCT version;
	ChessBoard guiCBBoard; // Gui board
	BYTE guiBoard[64]; // Gui board in dgt format
	BYTE stableBoard[64]; // Last stable board
	BYTE lastBoard[64]; // Last received board from the DGT board
	DGT_MSG_BWTIME_STRUCT clock;
	DGT_MSG_FIELD_UPDATE_STRUCT field;
	QString trademark;
	int busadres;
	int _status;
	QString serialnr;
	QComboBox* portlist;
	QSerialPort* port;
	QLabel* statusLine;
	QLabel* dgtVersion;
	QLabel* whiteClock;
	QLabel* blackClock;
	BoardWindow* boardwindow;
	QTimer* timer;
	void interpretMessage(BYTE code, int datalength, BYTE* data);
	void write(BYTE command);
	void updateDialog(BYTE*);
	void updateDialog(BYTE fiield, BYTE piece);
	bool equalBoard(BYTE*, BYTE*);
	bool equalBoard(ChessBoard&, BYTE*);
	void convertBoard(ChessBoard& from, BYTE* to);
	// Search to find the moves between the two board
	void findPossibleMoves(MoveList*, ChessBoard& start, ChessBoard& end);
	ChessMove getLegalMove(BYTE*);
};
