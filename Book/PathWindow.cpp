#include "PathWindow.h"
#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QButtonGroup>

PathWindow::PathWindow(QWidget *parent)
	: QWidget(parent)
{
	char sz[16];
//	QBoxLayout* box = new QBoxLayout(QBoxLayout::LeftToRight);
	QButtonGroup* box = new QButtonGroup(this);
	for (int i = 0; i < 60; i++)
	{
//		box->addWidget(new QLabel(itoa(i,sz,10)));
		box->addButton(new QPushButton(itoa(i, sz, 10),this));
	}
//	setLayout(box);
}

PathWindow::~PathWindow()
{
}

void PathWindow::update(QChessGame& game)
{
}
