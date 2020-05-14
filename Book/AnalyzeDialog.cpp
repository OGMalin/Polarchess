#include "AnalyzeDialog.h"
#include <QBoxLayout>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QTextStream>

AnalyzeDialog::AnalyzeDialog(QWidget* parent, Computer* c, Database* t, Database* w, Database* b, EngineWindow* e, BoardWindow* bw, Path* p)
	:QDialog(parent)
{
	QHBoxLayout* hbox;
	QVBoxLayout* vbox;
	QVBoxLayout* vbox2;
	QLabel* label;
	compDB = c;
	base[0] = t;
	base[1] = w;
	base[2] = b;
	enginewindow = e;
	boardwindow = bw;
	path = p;
	running = false;
	currentPosition = 0;

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

	label = new QLabel(tr("Time to use:"));
	timeToUse = new QSpinBox;
	timeToUse->setValue(60);
	timeToUse->setMaximum(999);
	timeToUse->setFixedWidth(60);

	vbox2 = new QVBoxLayout;
	allPosition = new QRadioButton(tr("All saved positions"));
	endPosition = new QRadioButton(tr("Only end positions"));
	currentPath = new QRadioButton(tr("Current path"));
	allPosition->setChecked(true);
	connect(currentPath, SIGNAL(toggled(bool)), this, SLOT(pathChanged(bool)));
	vbox2->addWidget(allPosition);
	vbox2->addWidget(endPosition);
	vbox2->addWidget(currentPath);

	hbox->addWidget(engineList);
	hbox->addWidget(dbList);
	hbox->addWidget(label);
	hbox->addWidget(timeToUse);
	hbox->addLayout(vbox2);
	vbox->addLayout(hbox);

	hbox = new QHBoxLayout;
	posLabel = new QLabel;

	hbox->addWidget(posLabel);
	vbox->addLayout(hbox);

	hbox = new QHBoxLayout;
	startButton = new QPushButton(tr("Start"));
	connect(startButton, SIGNAL(clicked()), this, SLOT(startAnalyze()));
	stopButton = new QPushButton(tr("Stop"));
	connect(stopButton, SIGNAL(clicked()), this, SLOT(stopAnalyze()));
	stopButton->setDisabled(true);
	closeButton = new QPushButton(tr("Close"));
	connect(closeButton, SIGNAL(clicked()), this, SLOT(closeAnalyze()));

	hbox->addWidget(startButton);
	hbox->addWidget(stopButton);
	hbox->addWidget(closeButton);
	vbox->addLayout(hbox);

	setLayout(vbox);

}

void AnalyzeDialog::startAnalyze()
{
	int i;
	if (running)
		return;
	QString eng = engineList->currentText();
	if (eng.isEmpty())
		return;
	currentPosition = 0;
	collectPositions();
	updateAnalyzed();
	if (!positions.size())
		return;
	boardwindow->setPosition(positions[currentPosition]);
	emit setPosition(positions[currentPosition]);
	connect(enginewindow, SIGNAL(enginePV(ComputerDBEngine&, ChessBoard&)), this, SLOT(enginePV(ComputerDBEngine&, ChessBoard&)));
	enginewindow->startAutomated(positions[currentPosition], eng, timeToUse->value());
	running = true;
	stopButton->setDisabled(false);
	startButton->setDisabled(true);
}

void AnalyzeDialog::stopAnalyze()
{
	disconnect(enginewindow, nullptr, nullptr, nullptr);
	enginewindow->stopAutomated();
	running = false;
	stopButton->setDisabled(true);
	startButton->setDisabled(false);
}

void AnalyzeDialog::closeAnalyze()
{
	if (running)
		stopAnalyze();
	close();
}

void AnalyzeDialog::enginePV(ComputerDBEngine& ce, ChessBoard& cb)
{
	compDB->add(ce, cb);
	++currentPosition;
	updateAnalyzed();
	if (currentPosition >= positions.size())
	{
		stopAnalyze();
		return;
	}
	QString eng = engineList->currentText();
	boardwindow->setPosition(positions[currentPosition]);
	enginewindow->startAutomated(positions[currentPosition], eng, timeToUse->value());
}

void AnalyzeDialog::collectPositions()
{
	ComputerDBEntry cde;
	QString qs;
	ComputerDBEngine ce;
	int i, j;
	int t = timeToUse->value() * 1000;
	ChessBoard cb;
	positions.clear();
	if (currentPath->isChecked())
	{
		i = path->current();
		for (j = 0; j <= i; j++)
		{
			path->current(j);
			cb = path->getPosition();
			positions.push_back(cb);
		}
	}
	else
	{
		i = dbList->currentIndex();
		if ((i < 0) && (i > 2))
			return;
		if (endPosition->isChecked())
			base[i]->getEndPositions(positions);
		else
			base[i]->getAllPositions(positions);
	}

	// Remove mate and stalemate
	for (i = 0; i < positions.size(); i++)
	{
		if (positions[i].legalMoves() == 0)
		{
			positions.remove(i);
			--i;
		}
	}
	qs = engineList->currentText();
	for (i = 0; i < positions.size(); i++)
	{
		cde = compDB->find(positions[i]);
		if (!cde.cboard.isEmpty())
		{
			if (cde.exist(qs, ce))
			{
				if (ce.time >= t)
				{
					positions.remove(i);
					--i;
				}
			}
		}
	}
}

void AnalyzeDialog::pathChanged(bool b)
{
	dbList->setDisabled(b);
}

void AnalyzeDialog::updateAnalyzed()
{
	QString qs;
	QTextStream(&qs) << tr("Analyzed: ") << currentPosition << "/" << positions.size();
	posLabel->setText(qs);
}