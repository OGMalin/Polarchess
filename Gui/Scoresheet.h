#pragma once

#include <QWidget>

class QLabel;
class QTableWidget;
class QChessGame;

class Scoresheet :public QWidget
{
	Q_OBJECT
private:
	QLabel * white;
	QLabel * black;
	QTableWidget* sheet;

public:
	Scoresheet(QWidget* parent = 0);
	void updateGame(QChessGame*);
};