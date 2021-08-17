#pragma once
// QSerialPort seems to lose bytes so I use Windows own serialport handling
//#define WINDOWS_SERIAL

#include <QThread>
#include <QSerialPortInfo>

#ifdef	WINDOWS_SERIAL
#include <Windows.h>
#else
#define BYTE unsigned char 
#include <QSerialPort>
#endif

class DgtSerial : public QThread
{
	Q_OBJECT
public slots:
	void readyRead();
	void errorOccurred(QSerialPort::SerialPortError);
signals:
	void resultReady(BYTE, int, BYTE*);
	void portOpen(bool);
public:
#ifdef	WINDOWS_SERIAL
	HANDLE hComm;
	HANDLE hThread;
#else
	QSerialPort* port;
#endif
	DgtSerial(QObject* parent = NULL);
	virtual ~DgtSerial();
	int write(BYTE b) { return write(&b, 1); };
	int write(BYTE* message, int length);
	bool connectDgt(QString portname);
	void disconnectDgt();
	void run() override;
	void setComPort(QString& qs) { comPort = qs; };
private:
	QString comPort;
};
