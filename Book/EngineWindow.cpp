#include "EngineWindow.h"
#include <QBoxLayout>

EngineWindow::EngineWindow(QWidget *parent)
	: QWidget(parent)
{
	QBoxLayout* box = new QBoxLayout(QBoxLayout::LeftToRight);
	setLayout(box);
}

EngineWindow::~EngineWindow()
{
}

void EngineWindow::update(ChessBoard& board)
{
}
