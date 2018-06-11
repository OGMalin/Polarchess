#pragma once

#include <QWidget>

class QTableWidget;

class MoveWindow : public QWidget
{
	Q_OBJECT

public:
	MoveWindow(QWidget *parent=0);
	~MoveWindow();

private:
	QTableWidget * moves;
};
