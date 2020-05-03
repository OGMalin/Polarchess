#include "../Common/DgtBoard.h"
#include "../Common/defs.h"
#include <QDialog>
#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QSerialPortInfo>
#include <QPushButton>
#include <QTextStream>
#include <QApplication>
#include <QByteArray>
#include <QDebug>

#ifdef _DEBUG
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
#endif

DgtBoard::DgtBoard(QWidget* parent)
	: QDialog(parent)
{
	port = new QSerialPort(this);
	connect(port, &QSerialPort::readyRead, this, &DgtBoard::readFromDGT);
	connect(port, SIGNAL(errorOccurred(int)), this, SLOT(comError(int)));
	QHBoxLayout* hbox;
	QVBoxLayout* vbox;
	_status = 0;

	timer = new QTimer();
	connect(timer, SIGNAL(timeout()), this, SLOT(timerMessage()));
	timer->start(500);

	hbox = new QHBoxLayout;

	vbox = new QVBoxLayout;

	QPushButton* button = new QPushButton(tr("Update port list")); // Do this automatic when you know how
	button->setFixedWidth(button->minimumSizeHint().width());
	connect(button, SIGNAL(clicked()), this, SLOT(listPorts()));
	vbox->addWidget(button);

	portlist = new QComboBox;
	listPorts();
	vbox->addWidget(portlist);

	button = new QPushButton(tr("Connect"));
	button->setFixedWidth(button->minimumSizeHint().width());
	connect(button, SIGNAL(clicked()), this, SLOT(connectDgt()));
	vbox->addWidget(button);

	button = new QPushButton(tr("Disconnect"));
	button->setFixedWidth(button->minimumSizeHint().width());
	connect(button, SIGNAL(clicked()), this, SLOT(disconnectDgt()));
	vbox->addWidget(button);

	statusLine = new QLabel("Disconnected");
	vbox->addWidget(statusLine);

	dgtVersion = new QLabel("Dgt Board version:");
	vbox->addWidget(dgtVersion);

	vbox->setAlignment(Qt::AlignTop);

	hbox->addLayout(vbox);

	boardwindow = new BoardWindow;
	boardwindow->setDisabled(true);
	boardwindow->setPosition("8/8/8/8/8/8/8/8 w - -");
	hbox->addWidget(boardwindow);

	vbox = new QVBoxLayout;
	vbox->addLayout(hbox);

	QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
	vbox->addWidget(buttons);

	setLayout(vbox);

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
	if (port->open(QIODevice::ReadWrite))
	{
		statusLine->setText("Connected");
		write(DGT_SEND_RESET);
		_sleep(200);
		QApplication::processEvents();
//		write(DGT_SEND_TRADEMARK);
		_sleep(200);
		QApplication::processEvents();
		write(DGT_SEND_VERSION);
		_sleep(200);
		QApplication::processEvents();
//		write(DGT_RETURN_SERIALNR);
		_sleep(200);
		QApplication::processEvents();
//		write(DGT_SEND_CLK);
		_sleep(200);
		QApplication::processEvents();
		write(DGT_SEND_BRD);
		_sleep(200);
		QApplication::processEvents();
		write(DGT_SEND_UPDATE_BRD);
		_sleep(200);
		QApplication::processEvents();
		_status = 1;
	}
	else
	{
		statusLine->setText("Disconnected");
		_status = 0;
	}
	emit dgtStatus(_status);
}

void DgtBoard::disconnectDgt()
{
	if (port->isOpen())
		port->close();
	_status = 0;
	emit dgtStatus(_status);
	statusLine->setText("Disconnected");
}

void DgtBoard::readFromDGT()
{
	BYTE code;
	int index, i, msglen, j;
	QByteArray data;
	BYTE d[0x3000];
	index = 0;
	while (1)
	{
		QApplication::processEvents();
		port->waitForReadyRead(100);
		data = port->readAll();
		if (!data.size())
			break;
#ifdef _DEBUG
		QString qs = port->portName() + " ->";
		qs += convertToHex(data);
		qDebug(qs.toLatin1());
#endif
		for (i=0;i<data.size();i++)
		{
			if (index == 0)
			{
				if ((data[i] & (BYTE)0x80) == 0)
					continue;
				code = data[i];
				++index;
			}
			else if (index == 1)
			{
				msglen = (data[i] & (BYTE)0x7f) << 7;
				++index;
			}
			else if (index == 2)
			{
				msglen += data[i] & (BYTE)0x7f;
				++index;
			}
			else
			{
				++index;
				if (index == msglen)
				{
					for (j = 0; j < data.size(); j++)
						d[j] = data[j];
					interpretMessage(code, msglen - 3, &d[3]);
					index = 0;
				}
			}

		}
	}
#ifdef _DEBUG
	if (index)
	{ // Missing bytes
		QString qs = port->portName() + " *->";
		qs += convertToHex(data);
		qDebug(qs.toLatin1());
	}
#endif
}

void DgtBoard::interpretMessage(BYTE code, int datalength, BYTE* data)
{
	MoveList* ml = new MoveList;
	ChessBoard bS;
	ChessBoard bL;
	int i,j;
	QString qs;
	switch (code)
	{
	case DGT_MSG_BOARD_DUMP:
		if (equalBoard(stableBoard, data))
			break;
		if (!equalBoard(lastBoard, data))
		{
			memcpy(lastBoard, data, 64);
			if (isVisible())
				updateDialog();
			break;
		}
		if (isVisible())
			updateDialog();
		memcpy(stableBoard, lastBoard, 64);
		convertBoard(bS, stableBoard);
		convertBoard(bL, lastBoard);
		findPossibleMoves(ml, bS, bL);
		emit newPosition(bS, *ml);
		break;
	case DGT_MSG_BWTIME:
		memset(&clock.leftFlag,data[0],1);
		clock.leftMinutes = data[1];
		clock.leftSeconds = data[2];
		memset(&clock.rightFlag, data[3], 1);
		clock.rightMinutes = data[4];
		clock.rightSeconds = data[5];
		memset(&clock.status, data[6], 1);
		break;
	case DGT_MSG_FIELD_UPDATE:
		field.field = data[0];
		field.piece = data[1];
//		if (field.field<64)
//			lastBoard[field.field] = field.piece;
//		updateDialog();
		break;
	case DGT_MSG_BUSADRES:
		busadres = (data[0] & (BYTE)0x7f) << 7;
		busadres += data[1] & (BYTE)0x7f;
		break;
	case DGT_MSG_SERIALNR:
		data[datalength] = '\0';
		serialnr = (char*)data;
		break;
	case DGT_MSG_TRADEMARK:
		data[datalength] = '\0';
		trademark = (char*)data;
		break;
	case DGT_MSG_VERSION:
		version.major = data[0];
		version.minor = data[1];
		qs= "Dgt Board version: ";
		QTextStream(&qs) << (int)version.major << "." << ((version.minor<10)?"0":"") << (int)version.minor;
		dgtVersion->setText(qs);
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
		_status = 0;
		return;
	}
	BYTE sz[2];
	sz[0] = command;
	sz[1] = 0;
	port->write((char*)sz);
#ifdef _DEBUG
	QString qs = port->portName() + " <- ";
	QTextStream(&qs) << QString::number(command,16);
	qDebug(qs.toLatin1());
#endif
	_sleep(3);
}

void DgtBoard::updateDialog()
{
	ChessBoard cb;
	convertBoard(cb, lastBoard);
	boardwindow->setPosition(cb);
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

void DgtBoard::comError(int error)
{

	qDebug("Error");
}

bool DgtBoard::equalBoard(BYTE*b1, BYTE*b2)
{
	for (int i = 0; i < 64; i++)
		if (b1[i] != b2[i])
			return false;
	return true;
}

bool DgtBoard::equalBoard(ChessBoard& cb, BYTE* b)
{
	return false;
}

void DgtBoard::convertBoard(ChessBoard& cb, BYTE* b)
{
	int dgtpiece[13] = { 0,1,4,2,3,6,5,7,10,8,9,12,11 };
	int i;
	int f, r;
	i = 0;
	for (r = 7; r > -1; r--)
	{
		for (f = 0; f < 8; f++)
		{
			if (b[i] < 13)
				cb.board[SQUARE(f, r)] = dgtpiece[b[i]];
			++i;
		}
	}
}

void DgtBoard::findPossibleMoves(MoveList*, ChessBoard& start, ChessBoard& end)
{

}

void DgtBoard::setBoard(ChessBoard& cb)
{
	guiBoard = cb;
	if (_status)
	{
		if (equalBoard(cb,stableBoard))
			_status=2;
		else
			_status=1;
	}
	emit dgtStatus(_status);
}