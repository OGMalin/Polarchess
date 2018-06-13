#pragma once

#include "Database.h"
#include <QWidget>

class QTableWidget;

class MoveWindow : public QWidget
{
	Q_OBJECT

public:
	MoveWindow(QWidget *parent=0);
	~MoveWindow();
	void update(BookDBEntry& data);

private:
	QTableWidget * moves;
};
