#include "EngineWindow.h"
#include <QCoreApplication>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTableView>

EngineWindow::EngineWindow(QWidget *parent)
	: QWidget(parent)
{
	multipv = 1;
	engine = new Engine;
	QString name = "Engine.eng";
	QString dir = QCoreApplication::applicationDirPath() + "/Engines";
	engine->setEngine(name, dir);

	QGridLayout* grid = new QGridLayout;

	QHBoxLayout* hbox = new QHBoxLayout;
	lines = new QLineEdit;
	decline = new QPushButton("-");
	incline = new QPushButton("+");
	analyze = new QPushButton("Analyze");
	freeze = new QPushButton("Freeze");
	hbox->addWidget(decline);
	hbox->addWidget(lines);
	hbox->addWidget(incline);
	hbox->addWidget(analyze);
	hbox->addWidget(freeze);
	grid->addLayout(hbox, 0, 0);

	QStandardItemModel* model = new QStandardItemModel(1, 3);
	QStringList header;
	header << "Score" << "Depth" << "PV";
	model->setHorizontalHeaderLabels(header);
	QTableView* output = new QTableView;
	output->setModel(model);

	grid->addWidget(output, 1, 0);// , Qt::AlignLeft);

	setLayout(grid);
}

EngineWindow::~EngineWindow()
{
	delete engine;
}

void EngineWindow::update(ChessBoard& board)
{
}
