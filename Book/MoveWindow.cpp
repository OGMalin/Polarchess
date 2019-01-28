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
	vbox->setMargin(0);
	vbox->addWidget(table);
	setLayout(vbox);
//	setContentsMargins(0, 0, 0, 0);
	normalBrush.setColor(Qt::black);
	repBrush.setColor(Qt::green);
}

MoveWindow::~MoveWindow()
{
}

void MoveWindow::update(BookDBEntry& theory, BookDBEntry& rep)
{
	int i,j;
	QString qs;
	QStandardItem* item;
	int rows;
	model->setRowCount(0);
	for (i = 0; i < theory.movelist.size();i++)
	{
		qs=QString(theory.board.makeMoveText(theory.movelist[i].move, FIDE).c_str());
		qs += theory.movelist[i].comment;
		item = new QStandardItem(qs);
		item->setEditable(false);
		item->setTextAlignment(Qt::AlignLeft);
		model->setItem(i,0,item);
	}
	rows = i;
	for (i = 0; i < rep.movelist.size(); i++)
	{
		qs = QString(rep.board.makeMoveText(rep.movelist[i].move, FIDE).c_str());
		qs += rep.movelist[i].comment;
		item = new QStandardItem(qs);
		item->setEditable(false);
		item->setTextAlignment(Qt::AlignLeft);
		if (rep.movelist[i].whiterep && (rep.board.toMove == WHITE))
			item->setForeground(repBrush);
		else if (rep.movelist[i].blackrep && (rep.board.toMove == BLACK))
			item->setForeground(repBrush);
		else
			item->setForeground(normalBrush);

		for (j = 0; j < theory.movelist.size(); j++)
		{
			if (rep.movelist[i].move == theory.movelist[j].move)
				break;
		}
		if (j < theory.movelist.size())
		{
			model->setItem(j, 1, item);
		}
		else
		{
			model->setItem(rows, 1, item);
			++rows;
		}
	}
	model->setRowCount(rows);
}
