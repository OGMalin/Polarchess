#pragma once

#include <QWidget>

class QChessGame;
class QTableWidget;

class Scoresheet :public QWidget
{
	Q_OBJECT
private:
	QTableWidget * sheet;
public:
	Scoresheet(QWidget* parent = 0);
	void updateGame(QChessGame*);
};