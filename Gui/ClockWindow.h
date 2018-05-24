#pragma once

#include <QWidget>
#include <QTextEdit>

class ClockWindow :public QTextEdit
{
	Q_OBJECT
public:
	ClockWindow(QWidget* parent = 0);
};