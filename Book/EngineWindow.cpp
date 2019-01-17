#include "EngineWindow.h"
#include <QGridLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>

EngineWindow::EngineWindow(QWidget *parent)
	: QWidget(parent)
{
	QGridLayout* grid = new QGridLayout;
	QTextEdit* output = new QTextEdit;
	QLineEdit* lines = new QLineEdit;
	QPushButton* analyze = new QPushButton("Analyze");
	grid->addWidget(lines, 0, 0, Qt::AlignLeft);
	grid->addWidget(analyze, 0, 1, Qt::AlignLeft);
	grid->addWidget(output, 1, 0, -1, -1, Qt::AlignLeft);

	setLayout(grid);
}

EngineWindow::~EngineWindow()
{
}

void EngineWindow::update(ChessBoard& board)
{
}
