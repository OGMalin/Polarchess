#include "Scoresheet.h"
#include "QChessGame.h"
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QStringList>
#include <QVBoxLayout>

Scoresheet::Scoresheet(QWidget* parent)
	:QWidget(parent)
{
	sheet = new QTableWidget(60,2);
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
	QFont f;
	f = item1->font();
	f.setBold(true);
	f.setPointSize(12);
	item1->setFont(f);
	item2->setFont(f);
	QList<QString> list;
	game->getMovelist(list);
	int i;
	int row = 0, col = 0;
	QTableWidgetItem* item;
	if (list.size()>120)
		sheet->setRowCount((list.size()+1)/2);
	for (i = 0; i < 10; i++)
	{
		item = new QTableWidgetItem("e4");
		item->setTextAlignment(Qt::AlignHCenter);
		sheet->setItem(row, col, item);
		++col;
		if (col > 1)
		{
			++row;
			col = 0;
		}
	}
}