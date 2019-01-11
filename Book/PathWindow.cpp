#include "PathWindow.h"
#include <QBoxLayout>

PathWindow::PathWindow(QWidget *parent)
	: QWidget(parent)
{
	QBoxLayout* box = new QBoxLayout(QBoxLayout::LeftToRight);
	setLayout(box);
}

PathWindow::~PathWindow()
{
}

void PathWindow::update(QChessGame& game)
{
}
