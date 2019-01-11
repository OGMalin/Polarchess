#include "OpeningWindow.h"
#include <QLineEdit>
#include <QHBoxLayout>

OpeningWindow::OpeningWindow(QWidget *parent)
	: QWidget(parent)
{
	QHBoxLayout* hbox = new QHBoxLayout;
	openingline = new QLineEdit;
	hbox->addWidget(openingline);
	setLayout(hbox);
}

OpeningWindow::~OpeningWindow()
{
}

void OpeningWindow::update(BookDBEntry& theory, BookDBEntry& rep)
{
	QString qs;
	if (theory.eco.isEmpty())
		qs += rep.eco + ": ";
	else
		qs += theory.eco + ": ";

	if (theory.opening.isEmpty())
		qs += rep.opening;
	else
		qs += theory.opening;
	openingline->setText(qs);
}
