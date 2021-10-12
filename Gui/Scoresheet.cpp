#include "Scoresheet.h"
#include "../Common/QChessGame.h"
#include <QTableView>
#include <QStandardItemModel>
#include <QStringList>
#include <QVBoxLayout>
#include <QHeaderView>

Scoresheet::Scoresheet(QWidget* parent)
	:QWidget(parent)
{
	QVBoxLayout* vbox = new QVBoxLayout;
	model = new QStandardItemModel(60, 2);
	table = new QTableView;
	table->setModel(model);
	QFont f;
	f = table->font();
	f.setPointSize(14);
	table->setFont(f);
	QHeaderView* h = table->horizontalHeader();
	f.setBold(true);
	h->setFont(f);

	vbox->addWidget(table);
	setLayout(vbox);
	
	connect(table, SIGNAL(clicked(const QModelIndex&)), this, SLOT(moveClicked(const QModelIndex&)));
	connect(table, SIGNAL(activated(const QModelIndex&)), this, SLOT(moveClicked(const QModelIndex&)));

//	resize(minimumSizeHint());
}

void Scoresheet::moveClicked(const QModelIndex& index)
{
	int col = index.column();
	int row = index.row();
	emit moveSelected(row * 2 + col + 1);
}

void Scoresheet::updateGame(QChessGame* game)
{
	QStringList header;
	header << game->white << game->black;
	model->setHorizontalHeaderLabels(header);
	model->clear();
	QStringList list;
	game->getMovelist(list);
	int i;
	int row = 0, col = 0;
	QStandardItem* item;
	if (list.size()>120)
		model->setRowCount((list.size()+1)/2);
	QStringList::iterator lit=list.begin();
	while(lit!=list.end())
	{
		item = new QStandardItem(*lit);
		item->setEditable(false);
		item->setTextAlignment(Qt::AlignCenter);
		model->setItem(row, col, item);
		++col;
		if (col > 1)
		{
			++row;
			col = 0;
		}
		++lit;
	}
	if (game->result!=0)
	{
		item = new QStandardItem(game->resultToString());
		item->setTextAlignment(Qt::AlignCenter);
		model->setItem(row, col, item);
	}
}