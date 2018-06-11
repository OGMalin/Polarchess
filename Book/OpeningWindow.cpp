#include "OpeningWindow.h"
#include <QLineEdit>
#include <QLabel>
#include <QGridLayout>

OpeningWindow::OpeningWindow(QWidget *parent)
	: QWidget(parent)
{
	QGridLayout* grid = new QGridLayout;
	opening = new QLineEdit;
	variation = new QLineEdit;
	subvariation = new QLineEdit;
	eco = new QLineEdit;
	QLabel* label1 = new QLabel("Opening");
	QLabel* label2 = new QLabel("Variation");
	QLabel* label3 = new QLabel("Subvariation");
	QLabel* label4 = new QLabel("Eco");
	grid->addWidget(label1, 0, 0);
	grid->addWidget(label2, 0, 1);
	grid->addWidget(label3, 0, 2);
	grid->addWidget(label4, 0, 3);
	grid->addWidget(opening, 1, 0);
	grid->addWidget(variation, 1, 1);
	grid->addWidget(subvariation, 1, 2);
	grid->addWidget(eco, 1, 3);

	setLayout(grid);
}

OpeningWindow::~OpeningWindow()
{
}
