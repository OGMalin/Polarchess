#include "OpeningWindow.h"
#include <QLineEdit>
#include <QLabel>
#include <QGridLayout>

OpeningWindow::OpeningWindow(QWidget *parent)
	: QLineEdit(parent)
{
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
	setText(qs);
}
