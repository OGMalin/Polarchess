#pragma once

#include <QWidget>
#include <QTimer>
#include <QLabel>

class Watch :public QWidget
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
	void pause(bool mark=true);
	void start();
	void stop();
	void restart();

public:
	Watch(QWidget* parent, bool enableContextMenu);
};