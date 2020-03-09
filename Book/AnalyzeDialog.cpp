#include "AnalyzeDialog.h"
#include <QBoxLayout>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QTextStream>

AnalyzeDialog::AnalyzeDialog(QWidget* parent, Computer* c, Database* t, Database* w, Database* b, EngineWindow* e, BoardWindow* bw, Path* p, Training* tdb)
	:QDialog(parent)
{
	QHBoxLayout* hbox;
	QVBoxLayout* vbox;
	QLabel* label;
	compDB = c;
	theoryDB = t;
	whiteDB = w;
	blackDB = b;
	trainingDB = tdb;
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

	endPosition = new QCheckBox(tr("Only end positions"));

	currentPath = new QCheckBox(tr("Current path"));
	connect(currentPath, SIGNAL(stateChanged(int)), this, SLOT(pathChanged(int)));

	hbox->addWidget(engineList);
	hbox->addWidget(dbList);
	hbox->addWidget(label);
	hbox->addWidget(timeToUse);
	hbox->addWidget(endPosition);
	hbox->addWidget(currentPath);
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
	int i, j;
	ChessBoard cb;
	positions.clear();
	if (currentPath->isChecked())
	{
		i = path->current();
		for (j = 0; j <= i; j++)
		{
			path->current(j);
			cb = path->getPosition();
			// Don't keep mate and stalemate
			if (cb.legalMoves() == 0)
				continue;
			positions.push_back(cb);
		}
		return;
	}
	Database* db;
	switch (dbList->currentIndex())
	{
	case 0:
		db = theoryDB;
		break;
	case 1:
		db = whiteDB;
		break;
	case 2:
		db = blackDB;
		break;
	default:
		return;
	}
	if (endPosition->isChecked())
	{
		if (db == theoryDB)
		{
			db->getEndPositions(positions);
		}
		else
		{
			// There isn't any endposition saved in a repertoire database, use the trainingdatabase instead.
			if (db == whiteDB)
				trainingDB->getEndpositions(positions, 0);
			else
				trainingDB->getEndpositions(positions, 1);
		}
	}
	else
	{
		db->getAllPositions(positions);
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
	ComputerDBEntry cde;
	QString qs = engineList->currentText();
	for (i = 0; i < positions.size(); i++)
	{
		if (positions[i].legalMoves() == 0)
			continue;
		cde = compDB->find(positions[i]);
		if (!cde.cboard.isEmpty())
		{
			if (cde.exist(qs))
			{
				positions.remove(i);
				--i;
			}

		}
	}
}

void AnalyzeDialog::pathChanged(int)
{
	if (currentPath->isChecked())
	{
		dbList->setDisabled(true);
		endPosition->setDisabled(true);
		return;
	}
	dbList->setDisabled(false);
	endPosition->setDisabled(false);
}

void AnalyzeDialog::updateAnalyzed()
{
	QString qs;
	QTextStream(&qs) << tr("Analyzed: ") << currentPosition << "/" << positions.size();
	posLabel->setText(qs);
}