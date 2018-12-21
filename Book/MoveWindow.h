#pragma once

#include "Database.h"
#include <QWidget>

class QTableView;
class QStandardItemModel;

class MoveWindow : public QWidget
{
	Q_OBJECT

public:
	MoveWindow(QWidget *parent=0);
	~MoveWindow();
	void update(BookDBEntry& theory, BookDBEntry& rep);

private:
	QTableView * table;
	QStandardItemModel* model;
};
