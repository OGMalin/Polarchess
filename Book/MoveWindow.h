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
	void addComment(QString& comment);

signals:
	void moveSelected(int rep, int movenr);
	void moveDelete(int rep, int movenr);
	void addMoveComment(int rep, int movenr, QString& comment);

public slots:
	void showContextMenu(const QPoint& pos);
	void selectFont();
	void deleteMove();
	void moveClicked(const QModelIndex& index);
	void addComment0() { addComment(QString("")); };
	void addComment1() { addComment(QString("!")); };
	void addComment2() { addComment(QString("?")); };
	void addComment3() { addComment(QString("!!")); };
	void addComment4() { addComment(QString("??")); };
	void addComment5() { addComment(QString("!?")); };
	void addComment6() { addComment(QString("?!")); };

private:
	Statistics* statistics;
	QTableView * table;
	QStandardItemModel* model;
	QBrush repBrush;
	QBrush normalBrush;
	QFont font;
};
