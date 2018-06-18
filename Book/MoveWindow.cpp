#include "MoveWindow.h"
#include <QTableView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QStringList>

MoveWindow::MoveWindow(QWidget *parent)
	: QWidget(parent)
{
	QVBoxLayout* vbox = new QVBoxLayout;
	model = new QStandardItemModel(0, 2);
	QStringList header;
	header << "Move" << "Score";
	model->setHorizontalHeaderLabels(header);
	table = new QTableView;
	table->setModel(model);
	vbox->addWidget(table);
	setLayout(vbox);
}

MoveWindow::~MoveWindow()
{
}

void MoveWindow::update(BookDBEntry& data)
{
}
