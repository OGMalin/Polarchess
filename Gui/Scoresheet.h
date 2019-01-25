#pragma once

#include <QWidget>

class QChessGame;
class QTableView;
class QStandardItemModel;

class Scoresheet :public QWidget
{
	Q_OBJECT
private:
	QTableView * table;
	QStandardItemModel* model;
public:
	Scoresheet(QWidget* parent = 0);
	void updateGame(QChessGame*);
};