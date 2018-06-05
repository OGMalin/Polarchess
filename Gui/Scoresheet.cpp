#include "Scoresheet.h"
#include "QChessGame.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include <QTableWidgetItem>

Scoresheet::Scoresheet(QWidget* parent)
	:QWidget(parent)
{
	// Create header
	white = new QLabel;
	black = new QLabel;
	sheet = new QTableWidget;
	QVBoxLayout* vbox = new QVBoxLayout;
	vbox->addWidget(white);
	vbox->addWidget(black);
	vbox->addWidget(sheet);
	setLayout(vbox);
}

void Scoresheet::updateGame(QChessGame* game)
{
	white->setText(game->white());
	black->setText(game->black());
	QList<QChessMove> moves = game->getMovelist();
	int i;
	QTableWidgetItem item("e4");
	int row = 0, col = 0;
	for (i = 0; i < 10; i++)
	{
		++col;
		sheet->setItem(row, col, &item);
		if (col > 1)
		{
			++row;
			col = 0;
		}
	}
}