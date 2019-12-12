#pragma once

#include <QWidget>
#include <QTimer>
#include <QLabel>

class StatusWatch :public QWidget
{
	Q_OBJECT

private:
	QTimer timer;
	int time;
	QLabel* text;
	int running;
public slots:
	void updateTime();
	void showContextMenu(const QPoint& pos);
	void pause();
	void start();
	void stop();

public:
	StatusWatch(QWidget* parent = 0);
};