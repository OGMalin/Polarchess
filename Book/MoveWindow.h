#pragma once

#include "Database.h"
#include <QWidget>
#include <QBrush>
#include <QFont>
#include "Statistics.h"

class QTableView;
class QStandardItemModel;

class MoveWindow : public QWidget
{
	Q_OBJECT

public:
	MoveWindow(QWidget *parent=0);
	~MoveWindow();
	void update(BookDBEntry& theory, BookDBEntry& white, BookDBEntry& black);
	void createStatistics();

signals:
	void moveSelected(int rep, int movenr);
	void moveDelete(int rep, int movenr);

public slots:
	void showContextMenu(const QPoint& pos);
	void selectFont();
	void deleteMove();
	void moveClicked(const QModelIndex& index);

private:
	Statistics* statistics;
	QTableView * table;
	QStandardItemModel* model;
	QBrush repBrush;
	QBrush normalBrush;
	QFont font;
};
