#pragma once

#include <QWidget>

class QLCDNumber;

class ClockWindow :public QWidget
{
	Q_OBJECT

private:
	QLCDNumber* clock[2];
	int time[2];
	int running;

public slots:
	void showtime();
signals:
	void clockAlarm(int);

public:
	ClockWindow(QWidget* parent = 0);
	void displaytime(int color);
	void settime(int w, int b);
	void start(int color);
};