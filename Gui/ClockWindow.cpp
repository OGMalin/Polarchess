#include "ClockWindow.h"
#include "../Common/defs.h"
#include <QLCDNumber>
#include <QHBoxLayout>
#include <QTimer>

ClockWindow::ClockWindow(QWidget* parent)
	:QWidget(parent)
{
	QHBoxLayout* hbox = new QHBoxLayout;
	int i;
	for (i = 0; i < 2; i++)
	{
		clock[i] = new QLCDNumber;
		clock[i]->setDigitCount(8);
		clock[i]->display("00:00.00");
		clock[i]->setStyleSheet("background-color:white;");
		hbox->addWidget(clock[i]);
	}
	setLayout(hbox);
	running = -1;
	time[WHITE]=time[BLACK] = 0;
	QTimer* timer=new QTimer(this);
	timer->start(500);
	connect(timer, SIGNAL(timeout()), this, SLOT(showtime()));
}

void ClockWindow::showtime()
{
	if (running < 0)
		return;
	time[running] -= 500;
	if (running>=0)
		displaytime(running);
}

void ClockWindow::displaytime(int color)
{
	char sz[16];
	int h,m,s,t;
	if (time[color]>0)
	{
		t = time[color] / 1000;
		h = t / 3600;
		m = (t - h * 3600) / 60;
		s = t - h * 3600 - m * 60;
		sprintf_s(sz, 16, "%02d:%02d:%02d", h, m, s);
		clock[color]->display(sz);
	}
	else
	{
		clock[color]->display("00:00:00");
		clock[color]->setStyleSheet("background-color:red;");
		emit clockAlarm(color);
		running = -1;
	}
}

void ClockWindow::settime(int w, int b)
{
	time[WHITE] = w;
	time[BLACK] = b;
	displaytime(WHITE);
	displaytime(BLACK);
}

void ClockWindow::start(int color)
{
	running = color;
	clock[WHITE]->setStyleSheet("background-color:white;");
	clock[BLACK]->setStyleSheet("background-color:white;");
}

int ClockWindow::gettime(int color)
{
	return time[color];
}

void ClockWindow::addtime(int t, int color)
{
	time[color]+=t;
}
