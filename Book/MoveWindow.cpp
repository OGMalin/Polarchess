#include "MoveWindow.h"
#include <QTableWidget>
#include <QVBoxLayout>

MoveWindow::MoveWindow(QWidget *parent)
	: QWidget(parent)
{
	moves = new QTableWidget(60, 3);
	QTableWidgetItem* item1 = new QTableWidgetItem("Move");
	QTableWidgetItem* item2 = new QTableWidgetItem("Score");
	QTableWidgetItem* item3 = new QTableWidgetItem("Delete");
	moves->setHorizontalHeaderItem(0, item1);
	moves->setHorizontalHeaderItem(1, item2);
	moves->setHorizontalHeaderItem(2, item3);
	QVBoxLayout* vbox = new QVBoxLayout;
	vbox->addWidget(moves);
	setLayout(vbox);
}

MoveWindow::~MoveWindow()
{
}
