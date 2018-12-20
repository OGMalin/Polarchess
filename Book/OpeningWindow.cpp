#include "OpeningWindow.h"
#include <QLineEdit>
#include <QLabel>
#include <QGridLayout>

OpeningWindow::OpeningWindow(QWidget *parent)
	: QWidget(parent)
{
	QGridLayout* grid = new QGridLayout;
	opening = new QLineEdit;
	eco = new QLineEdit;
	QLabel* label1 = new QLabel("Opening");
	QLabel* label2 = new QLabel("Eco");
	grid->addWidget(label1, 0, 0);
	grid->addWidget(label2, 0, 1);
	grid->addWidget(opening, 1, 0);
	grid->addWidget(eco, 1, 1);

	setLayout(grid);
}

OpeningWindow::~OpeningWindow()
{
}

void OpeningWindow::update(BookDBEntry& data)
{
	opening->setText(data.opening);
	eco->setText(data.eco);
}
