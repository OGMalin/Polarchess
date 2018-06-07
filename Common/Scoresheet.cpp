#include "Scoresheet.h"
#include "QChessGame.h"
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QStringList>
#include <QVBoxLayout>
#include <QHeaderView>

Scoresheet::Scoresheet(QWidget* parent)
	:QWidget(parent)
{
	sheet = new QTableWidget(60,2);
	sheet->setColumnWidth(0, 200); // Maybe look for some automatic adjustement
	sheet->setColumnWidth(1, 200);
	QFont f;
	f = sheet->font();
	f.setPointSize(14);
	sheet->setFont(f);
	QHeaderView* h = sheet->horizontalHeader();
	f.setBold(true);
	h->setFont(f);

	QVBoxLayout* vbox = new QVBoxLayout;
	vbox->addWidget(sheet);
	setLayout(vbox);
	
//	resize(minimumSizeHint());
}

void Scoresheet::updateGame(QChessGame* game)
{
	QTableWidgetItem* item1 = new QTableWidgetItem(game->white());
	QTableWidgetItem* item2 = new QTableWidgetItem(game->black());
	sheet->setHorizontalHeaderItem(0, item1);
	sheet->setHorizontalHeaderItem(1, item2);
	QStringList list;
	game->getMovelist(list);
	int i;
	int row = 0, col = 0;
	QTableWidgetItem* item;
	if (list.size()>120)
		sheet->setRowCount((list.size()+1)/2);
	QStringList::iterator lit=list.begin();
	item = sheet->itemAt(0, 0);
	while(lit!=list.end())
	{
		item = new QTableWidgetItem(*lit);
		item->setTextAlignment(Qt::AlignCenter);
		sheet->setItem(row, col, item);
		++col;
		if (col > 1)
		{
			++row;
			col = 0;
		}
		++lit;
	}
	sheet->scrollToItem(item);
}