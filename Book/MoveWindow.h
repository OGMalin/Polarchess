#pragma once

#include "Database.h"
#include <QWidget>
#include <QBrush>
#include <QFont>

class QTableView;
class QStandardItemModel;

class MoveWindow : public QWidget
{
	Q_OBJECT

public:
	MoveWindow(QWidget *parent=0);
	~MoveWindow();
	void update(BookDBEntry& theory, BookDBEntry& white, BookDBEntry& black);

public slots:
	void showContextMenu(const QPoint& pos);
	void selectFont();

private:
	QTableView * table;
	QStandardItemModel* model;
	QBrush repBrush;
	QBrush normalBrush;
	QFont font;
};
