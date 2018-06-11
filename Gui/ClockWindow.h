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
	// One clock run out of time.
	void clockAlarm(int /*color*/);

public:
	ClockWindow(QWidget* parent = 0);
	void displaytime(int color);
	void settime(int w, int b);
	void start(int color);
	void stop();
	// Return time in ms
	int gettime(int color);
	void addtime(int t, int color);
};