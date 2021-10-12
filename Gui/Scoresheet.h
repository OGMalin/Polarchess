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
signals:
	void moveSelected(int);
public slots:
	void moveClicked(const QModelIndex&);
public:
	Scoresheet(QWidget* parent = 0);
	void updateGame(QChessGame*);
};