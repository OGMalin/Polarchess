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

void OpeningWindow::refresh(BookDBEntry& theory, BookDBEntry& rep)
{
	QString qs;
	openingline->setText(qs);
}
