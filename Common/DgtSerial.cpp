#include <QSettings>
#include <QTextStream>
#include "../Common/DgtSerial.h"
#include "../Common/DgtProtocol.h"

QString convertToHex(char* data, int len)
{
	QString qs;
	for (int i = 0; i < len; i++)
		QTextStream(&qs) << ((data[i] < 16) ? " 0" : " ") << QString::number(data[i], 16);
	return qs;
}
QString convertToHex(QByteArray& data)
{
	QString qs;
	for (int i = 0; i < data.size(); i++)
		QTextStream(&qs) << (((BYTE)data[i] < 16) ? " 0" : " ") << QString::number((BYTE)data[i], 16);
	return qs;
}

DgtSerial::DgtSerial(QObject* parent)
	: QThread(parent)
{
#ifdef WINDOWS_SERIAL
	hComm = NULL;
#endif
}

DgtSerial::~DgtSerial()
{

}

void DgtSerial::run()
{
	if (comPort.isEmpty())
	{
		qDebug("DGT: Error, comport not defined.");
		return;
	}
	port = new QSerialPort();
	connect(port, &QSerialPort::readyRead, this, &DgtSerial::readyRead);
	connect(port, SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this, SLOT(errorOccurred(QSerialPort::SerialPortError)));
	port->setPortName(comPort);
	port->setBaudRate(QSerialPort::Baud9600);
	port->setDataBits(QSerialPort::Data8);
	port->setStopBits(QSerialPort::OneStop);
	port->setParity(QSerialPort::NoParity);
	port->setFlowControl(QSerialPort::NoFlowControl);
	if (port->open(QIODevice::ReadWrite))//| QIODevice::Unbuffered))
		_sleep(1500);
	emit portOpen(port->isOpen());
}

void DgtSerial::readyRead()
{
	char b;
	char buffer[0x4000];
	int byteRead;
	int index;
	int msgLen;

	index = 0;
	while (1)
	{
		byteRead = port->read(&b, 1);
		if (byteRead != 1)
		{
			break;
		}
		if (index == 0)
		{
			qDebug("DGT -> Code: %x", b);
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
			qDebug("DGT -> Msg length: %i", msgLen);
		}
		
		index++;
		if (index >= msgLen)
		{
			qDebug(QString("DGT -> " + convertToHex(&buffer[3], msgLen-3)).toLatin1());
			emit resultReady(buffer[0], msgLen - 3, (BYTE*)&buffer[3]);
			index = 0;
			msgLen = 0x4000;
		}
	}
	//	while (port->bytesAvailable())
	//	{
	//		if (port->read(d, 1) != 1)
	//			return;
	//		code = (BYTE)d[0];
	//
	//		qDebug(QString(port->portName() + QString(" -> Code: ") + QString(itoa(code, d, 16))).toLatin1());
	//
	//		//	eventLoop.exec();
	//
	////		QApplication::processEvents();
	//		if (port->read(d, 2) != 2)
	//			return;
	//		datalen = MSG_LENGTH(d[0], d[1]);
	//
	////		msglen = ((BYTE)d[0] & (BYTE)0x7f) << 7;
	////		msglen += (BYTE)d[1] & (BYTE)0x7f;
	//
	//		// Msg length include command and msg length bytes.
	//		datalen -= 3;
	//
	//		qDebug(QString(port->portName() + QString(" -> Msglen: ") + QString(itoa(datalen, d, 16))).toLatin1());
	//
	//		//	eventLoop.exec();
	//		if (datalen > 0)
	//		{
	////			QApplication::processEvents();
	//			if (port->read(d, datalen) != datalen)
	//			{
	//				qDebug("Can't read the message, not enough bytes.");
	//				return;
	//			}
	//		}
	//
	//		qDebug(QString(port->portName() + QString(" -> ") + convertToHex((BYTE*)d, datalen)).toLatin1());
	//
	//		interpretMessage(code, datalen, (BYTE*)d);
	////		_sleep(100);
	//	}
}

void DgtSerial::errorOccurred(QSerialPort::SerialPortError error)
{
	if (error)
		qDebug("DgtSerial Error: %i", error);
}

int DgtSerial::write(BYTE* message, int length)
{
	int written=0;

#ifdef WINDOWS_SERIAL
	if (!hComm)
	{
		qDebug("DGT <- Error: Try to write when port is not open.");
		return 0;
	}
	WriteFile(hComm, message, length, &written, NULL);
#else
	if (!port->isOpen())
	{
		qDebug("DGT <- Error: Try to write when port is not open.");
		return 0;
	}
	written=port->write((char*)message, length);
	port->flush();
#endif
	qDebug(QString(QString("DGT <- ") + convertToHex((char*)message, length)).toLatin1());
	if (written != length)
		qDebug("DGT -> Try to write %i bytes but did only write %i bytes.", length, written);
	return written;
}

bool DgtSerial::connectDgt(QString portname)
{
//	DCB dcb;
//	COMMTIMEOUTS timeout;
//	DWORD Maske;
//	QSettings settings;
//	disconnectDgt();
//	if (portname.isEmpty())
//		return false;
////		portname = portlist->currentText();
//	hComm = CreateFileA(
//		portname.toLatin1(),
//		GENERIC_READ | GENERIC_WRITE,
//		0,
//		NULL,
//		OPEN_EXISTING,
//		0,//FILE_FLAG_OVERLAPPED,
//		NULL);
//	if (hComm == INVALID_HANDLE_VALUE)
//	{
//		hComm = NULL;
//		return;
//	};
//
//	// Set up communication parameters.
//	dcb.DCBlength = sizeof(DCB);
//	GetCommState(hComm, &dcb);
//	dcb.BaudRate = CBR_9600;
//	dcb.ByteSize = 8;
//	dcb.Parity = NOPARITY;
//	dcb.StopBits = ONESTOPBIT;
//	dcb.fTXContinueOnXoff = 1;
//	dcb.fDtrControl = DTR_CONTROL_DISABLE;
//	dcb.fBinary = 1;
//	dcb.fParity = 0;
//	dcb.fOutxCtsFlow = 0;
//	dcb.fOutxDsrFlow = 0;
//	dcb.fDsrSensitivity = 0;
//	dcb.fOutX = 0;
//	dcb.fInX = 0;
//	dcb.fErrorChar = 0;
//	dcb.fNull = 0;
//	dcb.fRtsControl = RTS_CONTROL_DISABLE;
//	dcb.fAbortOnError = 0;
//	SetCommState(hComm, &dcb);
//
//	GetCommTimeouts(hComm, &timeout);
//	timeout.ReadIntervalTimeout = MAXDWORD;
//	timeout.ReadTotalTimeoutMultiplier = MAXDWORD;
//	timeout.ReadTotalTimeoutConstant = 500;
//	timeout.WriteTotalTimeoutMultiplier = 0;
//	timeout.WriteTotalTimeoutConstant = 0;
//
//	SetCommTimeouts(hComm, &timeout);
//	if (!GetCommMask(hComm, &Maske))
//		return;
//	Maske |= EV_RXCHAR;
//	//	if (!SetCommMask(hComm,Maske))
//	//   	return false;
//	//	return true;
//
//	  // Make an input thread
//	abort = false;
//
//	hThread = (HANDLE)_beginthread(DgtBoard::threadLoop, 0, this);
//	if (hThread == (HANDLE)-1)
//	{
//		hThread = NULL;
//		abort = true;
//		return;
//	}
//	settings.beginGroup("Dgt");
//	settings.setValue("port", portname);
//	settings.endGroup();
//
//	// Give the DGT board some time to initilize (is it needed?).
//	_sleep(1500);
//	updateMode = DGT_SEND_RESET;
//	write(updateMode);
//
//	write(DGT_SEND_VERSION);
	return false;
}

void DgtSerial::disconnectDgt()
{
	//abort = true;
	//if (hThread)
	//	if (WaitForSingleObject(hThread, 1000) == WAIT_TIMEOUT)
	//		TerminateThread(hThread, TRUE);
	//if (hComm != NULL)
	//	CloseHandle(hComm);
	//hThread = NULL;
	//hComm = NULL;
	//_status = 0;
	////	stableTimer->stop();
	//emit dgtStatus(_status);
	//statusLine->setText("Disconnected");
	//lBoardVersion->setText("No board");
	//lClockVersion->setText("No clock");
	//updateMode = 0;
	//clockConnected = false;
}

