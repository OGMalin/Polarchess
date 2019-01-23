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
	header << "Theory" << "Repertoire";
	model->setHorizontalHeaderLabels(header);
	table = new QTableView;
	table->setModel(model);
	vbox->addWidget(table);
	setLayout(vbox);
}

MoveWindow::~MoveWindow()
{
}

void MoveWindow::update(BookDBEntry& theory, BookDBEntry& rep)
{
	int i;
	QString qs;
	QStandardItem* item;
	for (i = 0; i < theory.movelist.size();i++)
	{
		qs=QString(theory.board.makeMoveText(theory.movelist[i].move, FIDE).c_str());
		qs += theory.movelist[i].comment;
		item = new QStandardItem(qs);
		item->setEditable(false);
		item->setTextAlignment(Qt::AlignLeft);
		model->setItem(i,0,item);
	}
	model->setRowCount(i);
}
