#include "AnalyzeDialog.h"
#include <QBoxLayout>
#include <QPushButton>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QLabel>

AnalyzeDialog::AnalyzeDialog(QWidget* parent, Computer* c, Database* t, Database* w, Database* b, EngineWindow* e, Path* p)
	:QDialog(parent)
{
	QHBoxLayout* hbox;
	QVBoxLayout* vbox;
	QPushButton* button;
	compDB = c;
	theoryDB = t;
	whiteDB = w;
	blackDB = b;
	enginewindow = e;
	path = p;

	vbox = new QVBoxLayout;

	hbox = new QHBoxLayout;

	engineList = new QComboBox;
	QString iniPath = QStandardPaths::locate(QStandardPaths::DocumentsLocation, QCoreApplication::organizationName(), QStandardPaths::LocateDirectory);
	iniPath += "/" + QCoreApplication::applicationName() + "/Engines";
	QFileInfoList qfil = QDir(iniPath, "*.eng").entryInfoList(QDir::NoFilter, QDir::Name | QDir::IgnoreCase);
	for (int i = 0; i < qfil.size(); i++)
		engineList->addItem(qfil[i].completeBaseName());
	if (compDB->enginelist.size())
		engineList->setCurrentText(compDB->enginelist.at(0));
	dbList = new QComboBox;
	dbList->addItem(tr("Theory"));
	dbList->addItem(tr("White repertoire"));
	dbList->addItem(tr("Black repertoire"));

	QLabel* label = new QLabel(tr("Time to use:"));
	timeToUse = new QSpinBox;
	timeToUse->setValue(60);
	timeToUse->setMaximum(999);
	timeToUse->setFixedWidth(60);

	endPosition = new QCheckBox(tr("Only end positions"));

	currentPath = new QCheckBox(tr("Current path"));

	hbox->addWidget(engineList);
	hbox->addWidget(dbList);
	hbox->addWidget(label);
	hbox->addWidget(timeToUse);
	hbox->addWidget(endPosition);
	hbox->addWidget(currentPath);

	vbox->addLayout(hbox);

	hbox = new QHBoxLayout;
	button = new QPushButton(tr("Start"));
	connect(button, SIGNAL(clicked()), this, SLOT(startAnalyze()));
	hbox->addWidget(button);
	button = new QPushButton(tr("Close"));
	connect(button, SIGNAL(clicked()), this, SLOT(closeAnalyze()));
	hbox->addWidget(button);
	vbox->addLayout(hbox);

	setLayout(vbox);

	connect(enginewindow, SIGNAL(enginePV(ComputerDBEngine&, ChessBoard&)), this, SLOT(enginePV(ComputerDBEngine&, ChessBoard&)));
}

void AnalyzeDialog::startAnalyze()
{

}

void AnalyzeDialog::closeAnalyze()
{

}

void AnalyzeDialog::enginePV(ComputerDBEngine& ce, ChessBoard& cb)
{
	compDB->add(ce, cb);
}

