#include "../Common/DgtBoard.h"
#include "../Common/defs.h"
#include <QDialog>
#include <QBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QDialogButtonBox>
#include <QSerialPortInfo>
#include <QPushButton>
#include <QTextStream>
#include <QApplication>
#include <QByteArray>
#include <QDebug>

//#ifdef _DEBUG
QString convertToHex(BYTE* data, int len)
{
	QString qs;
	for (int i=0;i<len;i++)
		QTextStream(&qs) << ((data[i]<16)?" 0":" ") << QString::number(data[i], 16);
	return qs;
}
QString convertToHex(QByteArray& data)
{
	QString qs;
	for (int i = 0; i < data.size(); i++)
		QTextStream(&qs) << (((BYTE)data[i] < 16) ? " 0" : " ") << QString::number((BYTE)data[i], 16);
	return qs;
}
//#endif

DgtBoard::DgtBoard(QWidget* parent)
	: QDialog(parent)
{
	memset(guiBoard, 0, 64);
	memset(stableBoard, 0, 64);
	memset(lastBoard, 0, 64);

	port = new QSerialPort(this);
//	connect(port, &QSerialPort::readyRead, this, &DgtBoard::readFromDGT);
	connect(port, &QSerialPort::readyRead, this, &DgtBoard::readyRead);
	connect(port, SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this, SLOT(comError(QSerialPort::SerialPortError)));

	setWindowTitle(tr("DGT board"));
	setSizeGripEnabled(true);
	QGridLayout* grid = new QGridLayout(this); //Main grid
	QFrame* frame = new QFrame(this);
	QHBoxLayout* hbox;
	QVBoxLayout* vbox;
	QPushButton* button;
	_status = 0;

	timer = new QTimer();
	timer->setSingleShot(true);
	connect(timer, SIGNAL(timeout()), this, SLOT(timerMessage()));

	vbox = new QVBoxLayout;

	hbox = new QHBoxLayout;
	button = new QPushButton(tr("Update port list")); // Do this automatic when you know how
	button->setFixedWidth(button->minimumSizeHint().width());
	connect(button, SIGNAL(clicked()), this, SLOT(listPorts()));
	hbox->addWidget(button);
	portlist = new QComboBox;
	listPorts();
	hbox->addWidget(portlist);
	vbox->addLayout(hbox);

	hbox = new QHBoxLayout;
	button = new QPushButton(tr("Connect"));
	button->setFixedWidth(button->minimumSizeHint().width());
	connect(button, SIGNAL(clicked()), this, SLOT(connectDgt()));
	hbox->addWidget(button);
	button = new QPushButton(tr("Disconnect"));
	button->setFixedWidth(button->minimumSizeHint().width());
	connect(button, SIGNAL(clicked()), this, SLOT(disconnectDgt()));
	hbox->addWidget(button);
	vbox->addLayout(hbox);
	statusLine = new QLabel("Disconnected");
	vbox->addWidget(statusLine);

	dgtVersion = new QLabel("Dgt Board version:");
	vbox->addWidget(dgtVersion);

//	vbox->setAlignment(Qt::AlignTop);

	grid->addLayout(vbox,0,0);

	vbox = new QVBoxLayout;

	hbox = new QHBoxLayout;
	whiteClock = new QLabel("00:00:00");
	blackClock = new QLabel("00:00:00");
	hbox->addWidget(whiteClock, 0, Qt::AlignLeft);
	hbox->addWidget(blackClock, 0, Qt::AlignRight);
	vbox->addLayout(hbox);
	boardwindow = new BoardWindow;
	boardwindow->setDisabled(true);
	boardwindow->setPosition("8/8/8/8/8/8/8/8 w - -");
	vbox->addWidget(boardwindow);
	grid->addLayout(vbox, 0, 1);

	QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
	grid->addWidget(buttons,1,0,1,2);

	connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
	setWindowFlag(Qt::WindowStaysOnTopHint);
}

DgtBoard::~DgtBoard()
{
	close();
}

int DgtBoard::status()
{
	return _status;
}

void DgtBoard::listPorts()
{
	portlist->clear();
	QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
	QList<QSerialPortInfo>::iterator it = ports.begin();
	while (it != ports.end())
	{
		portlist->addItem(it->portName());
		++it;
	}
	portlist->setFixedWidth(portlist->minimumSizeHint().width());
}

void DgtBoard::connectDgt()
{
	if (port->isOpen())
		port->close();
	QString com = portlist->currentText();
	QByteArray ba;
	port->setPortName(com);
	port->setBaudRate(QSerialPort::Baud9600);
	port->setDataBits(QSerialPort::Data8);
	port->setStopBits(QSerialPort::OneStop);
	port->setParity(QSerialPort::NoParity);
	port->setFlowControl(QSerialPort::NoFlowControl);
	if (port->open(QIODevice::ReadWrite))//| QIODevice::Unbuffered))
	{
		//port->setBreakEnabled(false);
		//port->setDataTerminalReady(false);
		//port->setRequestToSend(false);
		_sleep(1500);
		write(DGT_SEND_RESET);
		write(DGT_SEND_VERSION);
	}
	else
	{
		statusLine->setText("Disconnected");
		_status = 0;
		emit dgtStatus(_status);
	}
}

void DgtBoard::disconnectDgt()
{
	if (port->isOpen())
		port->close();
	_status = 0;
	timer->stop();
	emit dgtStatus(_status);
	statusLine->setText("Disconnected");
}

void DgtBoard::readyRead()
{
	BYTE code;
	int index, i, msglen, j;
	QByteArray data;
	char d[0x3000];
	index = 0;
	QApplication::processEvents();

//	eventLoop.exec();

	if (!port->isOpen())
		return;
	_sleep(100);
	while (port->bytesAvailable())
	{
		if (port->read(d, 1) != 1)
			return;
		code = (BYTE)d[0];

		qDebug(QString(port->portName() + QString(" -> Code: ") + QString(itoa(code, d, 16))).toLatin1());

		//	eventLoop.exec();

		QApplication::processEvents();
		if (port->read(d, 2) != 2)
			return;

		msglen = ((BYTE)d[0] & (BYTE)0x7f) << 7;
		msglen += (BYTE)d[1] & (BYTE)0x7f;

		msglen -= 3;

		qDebug(QString(port->portName() + QString(" -> Msglen: ") + QString(itoa(msglen, d, 16))).toLatin1());

		//	eventLoop.exec();
		if (msglen > 0)
		{
			QApplication::processEvents();
			if (port->read(d, msglen) != msglen)
			{
				qDebug("Can't read the message, not enough bytes.");
				return;
			}
		}

		qDebug(QString(port->portName() + QString(" -> ") + convertToHex((BYTE*)d, msglen)).toLatin1());

		interpretMessage(code, msglen, (BYTE*)d);
		_sleep(100);
	}
}

//void DgtBoard::readFromDGT()
//{
//	BYTE code;
//	int index, i, msglen, j;
//	QByteArray data;
//	BYTE d[0x3000];
//	index = 0;
//	while (1)
//	{
//		QApplication::processEvents();
//		port->waitForReadyRead(500);
//		data = port->readAll();
//		if (!data.size())
//			break;
//		qDebug(QString(port->portName() + QString(" -> ") + convertToHex(data)).toLatin1());
//		msglen = 0;
//		for (i=0;i<data.size();i++)
//		{
//			if (index == 0)
//			{
//				if ((data[i] & (BYTE)0x80) == 0)
//					continue;
//				code = data[i];
//				++index;
//			}
//			else if (index == 1)
//			{
//				msglen = (data[i] & (BYTE)0x7f) << 7;
//				++index;
//			}
//			else if (index == 2)
//			{
//				msglen += data[i] & (BYTE)0x7f;
//				++index;
//			}
//			else
//			{
//				++index;
//				if (index == msglen)
//				{
//					for (j = 0; j < data.size(); j++)
//						d[j] = data[j];
//					interpretMessage(code, msglen - 3, &d[3]);
//					index = 0;
//					msglen = 0;
//				}
//			}
//
//		}
//		break;
//	}
//	if (index)
//		qDebug(QString(port->portName() + QString(" *-> ") + convertToHex(data)).toLatin1());
//}

void DgtBoard::interpretMessage(BYTE code, int datalength, BYTE* data)
{
	MoveList* ml = new MoveList;
	ChessBoard bS;
	ChessBoard bL;
	ChessMove move;
	int i,j;
	QString qs;
	switch (code)
	{
	case DGT_MSG_BOARD_DUMP:
		if (datalength != DGT_MSG_BOARD_DUMP_SIZE)
		{
			qDebug("DGT_MSG_BOARD_DUMP messagelength mismatch");
			break;
		}
		move = getLegalMove(data);
		if (move.empty())
			break;

		memcpy(lastBoard, data, 64);
		if (timer->isActive())
		{
			updateDialog(lastBoard);
			write(DGT_SEND_BRD);
			break;
		}
		if (equalBoard(stableBoard, lastBoard))
			break;
		memcpy(stableBoard, lastBoard, 64);
		emit newMove(move);
		break;
	case DGT_MSG_BWTIME:
		if (datalength != DGT_MSG_BWTIME_SIZE)
		{
			qDebug("DGT_MSG_BWTIME messagelength mismatch");
			break;
		}
		memset(&clock.leftFlag,data[0],1);
		clock.leftMinutes = data[1];
		clock.leftSeconds = data[2];
		memset(&clock.rightFlag, data[3], 1);
		clock.rightMinutes = data[4];
		clock.rightSeconds = data[5];
		memset(&clock.status, data[6], 1);
		break;
	case DGT_MSG_FIELD_UPDATE:
		if (datalength != DGT_MSG_FIELD_UPDATE_SIZE)
		{
			qDebug("DGT_MSG_FIELD_UPDATE messagelength mismatch");
			break;
		}
		timer->stop();
		write(DGT_SEND_BRD);
//		timer->start(500);
		//field.field = data[0];
		//field.piece = data[1];
//		if (field.field<64)
//			lastBoard[field.field] = field.piece;
		updateDialog(data[0],data[1]);
		break;
	case DGT_MSG_BUSADRES:
		if (datalength != DGT_MSG_BUSADRES_SIZE)
		{
			qDebug("DGT_MSG_BUSADRES messagelength mismatch");
			break;
		}
		busadres = (data[0] & (BYTE)0x7f) << 7;
		busadres += data[1] & (BYTE)0x7f;
		break;
	case DGT_MSG_SERIALNR:
		if (datalength != DGT_MSG_SERIALNR_SIZE)
		{
			qDebug("DGT_MSG_SERIALNR messagelength mismatch");
			break;
		}
		data[datalength] = '\0';
		serialnr = (char*)data;
		break;
	case DGT_MSG_TRADEMARK:
		data[datalength] = '\0';
		trademark = (char*)data;
		break;
	case DGT_MSG_VERSION:
		// Version are used to check for connection.
		if (datalength != DGT_MSG_VERSION_SIZE)
		{
			qDebug("DGT_MSG_VERSION messagelength mismatch");
			break;
		}
		version.major = data[0];
		version.minor = data[1];
		qs= "Dgt Board version: ";
		QTextStream(&qs) << (int)version.major << "." << ((version.minor<10)?"0":"") << (int)version.minor;
		dgtVersion->setText(qs);
		statusLine->setText("Connected");
		_status = 1;
		emit dgtStatus(_status);
		write(DGT_SEND_CLK);
		write(DGT_SEND_BRD);
		write(DGT_SEND_UPDATE_NICE);
		break;
	case DGT_MSG_ERROR:
	case DGT_MSG_NONE:
	default:
		break;
	}
	delete ml;
}

void DgtBoard::write(BYTE command)
{
	if (_status == 0)
		return;
	if (!port->isOpen())
	{
		qDebug(QString(port->portName() + "*<- Error: Try to write when port is not open.").toLatin1());
		_status = 0;
		return;
	}
	port->write((char*)&command, 1);
	port->flush();

	qDebug(QString(port->portName() + QString(" <- ") + convertToHex(&command,1)).toLatin1());
	_sleep(100);
	QApplication::processEvents();
}

void DgtBoard::updateDialog(BYTE* data)
{
	ChessBoard cb;
	convertBoard(cb, lastBoard);
	boardwindow->setPosition(cb);
}

void DgtBoard::updateDialog(BYTE field, BYTE piece)
{
	int p[13] = { EMPTY,whitepawn,whiterook,whiteknight,whitebishop,whiteking,whitequeen,blackpawn,blackrook,blackknight,blackbishop,blackking,blackqueen };
	int sq[64] = {  56,57,58,59,60,61,62,63,
					48,49,50,51,52,53,54,55,
					40,41,42,43,44,45,46,47,
					32,33,34,35,36,37,38,39,
					24,25,26,27,28,29,30,31,
					16,17,18,19,20,21,22,23,
					 8, 9,10,11,12,13,14,15,
					 0, 1, 2, 3, 4, 5, 6, 7 };
	if ((field > 63) || (piece > 12))
		return;
	boardwindow->setPiece(sq[field], p[piece]);
}

void DgtBoard::timerMessage()
{
	if (port->isOpen())
	{
		write(DGT_SEND_BRD);
	}
	else
	{
		// Test for connection
	}
}

void DgtBoard::comError(QSerialPort::SerialPortError  error)
{
	if (error)
		qDebug("Error: %i, %s", error, port->errorString().toLatin1());
//	port->clearError();
}

bool DgtBoard::equalBoard(BYTE*b1, BYTE*b2)
{
	for (int i = 0; i < 64; i++)
		if (b1[i] != b2[i])
			return false;
	return true;
}

bool DgtBoard::equalBoard(ChessBoard& cb, BYTE* data)
{
	BYTE b[64];
	convertBoard(cb, b);
	return equalBoard(b, data);
}

// Convert from ChessBoard to DGT board
void DgtBoard::convertBoard(ChessBoard& cb, BYTE* b)
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


void DgtBoard::findPossibleMoves(MoveList*, ChessBoard& start, ChessBoard& end)
{

}

void DgtBoard::setBoard(ChessBoard& cb)
{
	guiCBBoard = cb;
	convertBoard(cb, guiBoard);
	if (_status)
	{
		if (equalBoard(guiBoard,stableBoard))
			_status=2;
		else
			_status=1;
	}
	emit dgtStatus(_status);
}

ChessMove DgtBoard::getLegalMove(BYTE* data)
{
	MoveList ml;
	ChessBoard cb;
	BYTE b[64];
	int i;
	guiCBBoard.legalMoves(ml);
	for (i = 0; i < ml.size(); i++)
	{
		cb = guiCBBoard;
		cb.doMove(ml[i],false);
		convertBoard(cb, b);
		if (equalBoard(data, b))
			return ml[i];
	}
	return ChessMove();
}