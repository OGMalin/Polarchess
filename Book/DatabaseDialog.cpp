#include "DatabaseDialog.h"
#include "ImportPgnDialog.h"
#include "ExportPgnDialog.h"
#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QPushButton>
#include <QFileDialog>
#include <QGridLayout>
#include <QSpacerItem>
#include <QMessageBox>
#include <QFile>

DatabaseDialog::DatabaseDialog(QWidget* parent, Database* theory, Database* white, Database* black, /*Training* training,*/ Computer* computer, Statistics* stat, Openings* opening)
	:QDialog(parent)
{
	QSpacerItem* spacer;
	theoryDB = theory;
	whiteDB = white;
	blackDB = black;
//	trainingDB = training;
	computerDB = computer;
	statDB = stat;
	openingsDB = opening;
	QVBoxLayout* vbox;
	QGroupBox* group;
	QPushButton* button;
	QGridLayout* grid;

	setWindowTitle(tr("Database setup"));
	setMinimumSize(200, 200);

	vbox = new QVBoxLayout;

	group = new QGroupBox(tr("Theory book"));
	grid = new QGridLayout();
	theoryFile = new QLineEdit(theoryDB->getPath());
	theoryFile->setDisabled(true);
	grid->addWidget(theoryFile, 0, 0, 1, -1);
	button = new QPushButton(tr("Open"));
	QSize spacersize = button->sizeHint();
	connect(button, SIGNAL(clicked()), this, SLOT(openTheoryDB()));
	grid->addWidget(button,1,0);
	button = new QPushButton(tr("New"));
	connect(button, SIGNAL(clicked()), this, SLOT(newTheoryDB()));
	grid->addWidget(button,1,1);
	button = new QPushButton(tr("Close"));
	connect(button, SIGNAL(clicked()), this, SLOT(closeTheoryDB()));
	grid->addWidget(button,1,2);
	button = new QPushButton(tr("Import Book"));
	connect(button, SIGNAL(clicked()), this, SLOT(importBookTheory()));
	button->setToolTip(tr("Import book into the theory."));
	grid->addWidget(button, 1, 3);
	button = new QPushButton(tr("Import Pgn"));
	connect(button, SIGNAL(clicked()), this, SLOT(importPGNTheory()));
	button->setToolTip(tr("Import pgn file into the theory."));
	grid->addWidget(button, 1, 4);
	button = new QPushButton(tr("Export Book"));
	connect(button, SIGNAL(clicked()), this, SLOT(exportPGNTheory()));
	button->setToolTip(tr("Export theory to pgn file."));
	grid->addWidget(button, 1, 5);
	button = new QPushButton(tr("Clean Book"));
	connect(button, SIGNAL(clicked()), this, SLOT(cleanupTheoryDB()));
	button->setToolTip(tr("Remove unused positions."));
	grid->addWidget(button, 1, 6);
	grid->setAlignment(Qt::AlignLeft);
	group->setLayout(grid);
	vbox->addWidget(group);

	group = new QGroupBox(tr("White repertoire book"));
	whiteFile = new QLineEdit(whiteDB->getPath());
	whiteFile->setDisabled(true);
	grid = new QGridLayout();
	grid->addWidget(whiteFile, 0, 0, 1, -1);
	button = new QPushButton(tr("Open"));
	connect(button, SIGNAL(clicked()), this, SLOT(openWhiteDB()));
	grid->addWidget(button, 1, 0);
	button = new QPushButton(tr("New"));
	connect(button, SIGNAL(clicked()), this, SLOT(newWhiteDB()));
	grid->addWidget(button, 1, 1);
	button = new QPushButton(tr("Close"));
	connect(button, SIGNAL(clicked()), this, SLOT(closeWhiteDB()));
	grid->addWidget(button, 1, 2);
	button = new QPushButton(tr("Import Book"));
	connect(button, SIGNAL(clicked()), this, SLOT(importBookWhite()));
	button->setToolTip(tr("Import book into the White repertoire."));
	grid->addWidget(button, 1, 3);
	button = new QPushButton(tr("Import Pgn"));
	connect(button, SIGNAL(clicked()), this, SLOT(importPGNWhite()));
	button->setToolTip(tr("Import pgn file into the White repertoire."));
	grid->addWidget(button, 1, 4);
	button = new QPushButton(tr("Export Book"));
	connect(button, SIGNAL(clicked()), this, SLOT(exportPGNWhite()));
	button->setToolTip(tr("Export White repertoire to pgn file."));
	grid->addWidget(button, 1, 5);
	button = new QPushButton(tr("Clean Book"));
	connect(button, SIGNAL(clicked()), this, SLOT(cleanupWhiteDB()));
	button->setToolTip(tr("Remove unused positions."));
	grid->addWidget(button, 1, 6);
	grid->setAlignment(Qt::AlignLeft);
	group->setLayout(grid);
	vbox->addWidget(group);

	group = new QGroupBox(tr("Black repertoire book"));
	grid = new QGridLayout();
	blackFile = new QLineEdit(blackDB->getPath());
	blackFile->setDisabled(true);
	grid->addWidget(blackFile, 0, 0, 1, -1);
	button = new QPushButton(tr("Open"));
	connect(button, SIGNAL(clicked()), this, SLOT(openBlackDB()));
	grid->addWidget(button, 1, 0);
	button = new QPushButton(tr("New"));
	connect(button, SIGNAL(clicked()), this, SLOT(newBlackDB()));
	grid->addWidget(button, 1, 1);
	button = new QPushButton(tr("Close"));
	connect(button, SIGNAL(clicked()), this, SLOT(closeBlackDB()));
	grid->addWidget(button, 1, 2);
	button = new QPushButton(tr("Import Book"));
	connect(button, SIGNAL(clicked()), this, SLOT(importBookBlack()));
	button->setToolTip(tr("Import book into the Black repertoire."));
	grid->addWidget(button, 1, 3);
	button = new QPushButton(tr("Import Pgn"));
	connect(button, SIGNAL(clicked()), this, SLOT(importPGNBlack()));
	button->setToolTip(tr("Import pgn file into the Black repertoire."));
	grid->addWidget(button, 1, 4);
	button = new QPushButton(tr("Export Book"));
	connect(button, SIGNAL(clicked()), this, SLOT(exportPGNBlack()));
	button->setToolTip(tr("Export Black repertoire to pgn file."));
	grid->addWidget(button, 1, 5);
	button = new QPushButton(tr("Clean Book"));
	connect(button, SIGNAL(clicked()), this, SLOT(cleanupBlackDB()));
	button->setToolTip(tr("Remove unused positions."));
	grid->addWidget(button, 1, 6);
	grid->setAlignment(Qt::AlignLeft);
	group->setLayout(grid);
	vbox->addWidget(group);

	group = new QGroupBox(tr("Computer evaluation"));
	grid = new QGridLayout();
	computerFile = new QLineEdit(computerDB->getPath());
	computerFile->setDisabled(true);
	grid->addWidget(computerFile, 0, 0, 1, -1);
	button = new QPushButton(tr("Open"));
	connect(button, SIGNAL(clicked()), this, SLOT(openComputerDB()));
	grid->addWidget(button, 1, 0);
	button = new QPushButton(tr("New"));
	connect(button, SIGNAL(clicked()), this, SLOT(newComputerDB()));
	grid->addWidget(button, 1, 1);
	button = new QPushButton(tr("Close"));
	connect(button, SIGNAL(clicked()), this, SLOT(closeComputerDB()));
	grid->addWidget(button, 1, 2);
	spacer = new QSpacerItem(spacersize.width(), spacersize.height());
	grid->addItem(spacer, 1, 3);
	spacer = new QSpacerItem(spacersize.width(), spacersize.height());
	grid->addItem(spacer, 1, 4);
	spacer = new QSpacerItem(spacersize.width(), spacersize.height());
	grid->addItem(spacer, 1, 5);
	spacer = new QSpacerItem(spacersize.width(), spacersize.height());
	grid->addItem(spacer, 1, 6);
	grid->setAlignment(Qt::AlignLeft);
	group->setLayout(grid);
	vbox->addWidget(group);

	group = new QGroupBox(tr("Games statistic"));
	grid = new QGridLayout();
	statisticFile = new QLineEdit(statDB->getPath());
	statisticFile->setDisabled(true);
	grid->addWidget(statisticFile, 0, 0, 1, -1);
	button = new QPushButton(tr("Open"));
	connect(button, SIGNAL(clicked()), this, SLOT(openStatisticDB()));
	grid->addWidget(button, 1, 0);
	button = new QPushButton(tr("New"));
	connect(button, SIGNAL(clicked()), this, SLOT(newStatisticDB()));
	grid->addWidget(button, 1, 1);
	button = new QPushButton(tr("Close"));
	connect(button, SIGNAL(clicked()), this, SLOT(closeStatisticDB()));
	grid->addWidget(button, 1, 2);
	button = new QPushButton(tr("Import"));
	connect(button, SIGNAL(clicked()), this, SLOT(importStatisticsDB()));
	button->setToolTip(tr("Import pgn file into the statistics."));
	grid->addWidget(button, 1, 3);
	button = new QPushButton(tr("Compact"));
	connect(button, SIGNAL(clicked()), this, SLOT(compactStatisticsDB()));
	button->setToolTip(tr("Remove positions with just a single game."));
	grid->addWidget(button, 1, 4);
	spacer = new QSpacerItem(spacersize.width(), spacersize.height());
	grid->addItem(spacer, 1, 5);
	spacer = new QSpacerItem(spacersize.width(), spacersize.height());
	grid->addItem(spacer, 1, 6);
	grid->setAlignment(Qt::AlignLeft);
	group->setLayout(grid);
	vbox->addWidget(group);

	group = new QGroupBox(tr("Openings"));
	grid = new QGridLayout();
	openingsFile = new QLineEdit(openingsDB->getPath());
	openingsFile->setDisabled(true);
	grid->addWidget(openingsFile, 0, 0, 1, -1);
	button = new QPushButton(tr("Open"));
	connect(button, SIGNAL(clicked()), this, SLOT(openOpeningsDB()));
	grid->addWidget(button, 1, 0);
	button = new QPushButton(tr("New"));
	connect(button, SIGNAL(clicked()), this, SLOT(newOpeningsDB()));
	grid->addWidget(button, 1, 1);
	button = new QPushButton(tr("Close"));
	connect(button, SIGNAL(clicked()), this, SLOT(closeOpeningsDB()));
	grid->addWidget(button, 1, 2);
	button = new QPushButton(tr("Import"));
	connect(button, SIGNAL(clicked()), this, SLOT(importOpeningsDB()));
	grid->addWidget(button, 1, 3);
	button = new QPushButton(tr("Export"));
	connect(button, SIGNAL(clicked()), this, SLOT(exportOpeningsDB()));
	grid->addWidget(button, 1, 4);
	spacer = new QSpacerItem(spacersize.width(), spacersize.height());
	grid->addItem(spacer, 1, 5);
	spacer = new QSpacerItem(spacersize.width(), spacersize.height());
	grid->addItem(spacer, 1, 6);
	grid->setAlignment(Qt::AlignLeft);
	group->setLayout(grid);
	vbox->addWidget(group);

	QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
	vbox->addWidget(buttons);
	setLayout(vbox);

	connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
}

void DatabaseDialog::openTheoryDB()
{
	QString path = QFileDialog::getOpenFileName(this, tr("Open theory book"), theoryFile->text(), tr("Book files (*.pbk)"));
	if (!path.isEmpty())
	{
		theoryDB->close();
		theoryFile->setText(path);
		if (theoryDB->open(path))
			theoryDB->create(path, 0);
	}
}

void DatabaseDialog::newTheoryDB()
{
	QString path = QFileDialog::getSaveFileName(this, tr("New theory book"), theoryFile->text(), tr("Book files (*.pbk)"));
	if (!path.isEmpty())
	{
		theoryDB->close();
		theoryFile->setText(path);
		if (!theoryDB->open(path))
			theoryDB->create(path, 0);
	}
}

void DatabaseDialog::closeTheoryDB()
{
	theoryDB->close();
}

void DatabaseDialog::cleanupTheoryDB()
{
	theoryDB->cleanUp();
}
void DatabaseDialog::openWhiteDB()
{
	QString path = QFileDialog::getOpenFileName(this, tr("Open White repertoire book"), whiteFile->text(), tr("Book files (*.pbk)"));
	if (!path.isEmpty())
	{
		whiteDB->close();
		whiteFile->setText(path);
		if (!whiteDB->open(path))
			whiteDB->create(path, 1);
	}
}

void DatabaseDialog::newWhiteDB()
{
	QString path = QFileDialog::getSaveFileName(this, tr("New White repertoire book"), whiteFile->text(), tr("Book files (*.pbk)"));
	if (!path.isEmpty())
	{
		whiteDB->close();
		whiteFile->setText(path);
		if (!whiteDB->open(path))
			whiteDB->create(path, 1);
	}
}

void DatabaseDialog::closeWhiteDB()
{
	whiteFile->setText("");
	whiteDB->close();
}

void DatabaseDialog::cleanupWhiteDB()
{
	whiteDB->cleanUp();
}

void DatabaseDialog::openBlackDB()
{
	QString path = QFileDialog::getOpenFileName(this, tr("Open Black repertoire book"), blackFile->text(), tr("Book files (*.pbk)"));
	if (!path.isEmpty())
	{
		blackDB->close();
		blackFile->setText(path);
		if (!blackDB->open(path))
			blackDB->create(path, 2);
	}
}

void DatabaseDialog::newBlackDB()
{
	QString path = QFileDialog::getSaveFileName(this, tr("New Black repertoire book"), blackFile->text(), tr("Book files (*.pbk)"));
	if (!path.isEmpty())
	{
		blackDB->close();
		blackFile->setText(path);
		if (!blackDB->open(path))
			blackDB->create(path, 2);
	}
}

void DatabaseDialog::closeBlackDB()
{
	blackFile->setText("");
	blackDB->close();
}

void DatabaseDialog::cleanupBlackDB()
{
	blackDB->cleanUp();
}

//void DatabaseDialog::openTrainingDB()
//{
//	QString path = QFileDialog::getOpenFileName(this, tr("Open training book"), trainingFile->text(), tr("Training files (*.ptr)"));
//	if (!path.isEmpty())
//	{
//		trainingDB->close();
//		trainingFile->setText(path);
//		if (!trainingDB->open(path))
//			trainingDB->create(path);
//	}
//}
//
//void DatabaseDialog::newTrainingDB()
//{
//	QString path = QFileDialog::getSaveFileName(this, tr("New training book"), trainingFile->text(), tr("Training files (*.ptr)"));
//	if (!path.isEmpty())
//	{
//		trainingDB->close();
//		trainingFile->setText(path);
//		if (!trainingDB->open(path))
//			trainingDB->create(path);
//	}
//}
//
//void DatabaseDialog::closeTrainingDB()
//{
//	trainingFile->setText("");
//	trainingDB->close();
//}
//
//void DatabaseDialog::clearTrainingDB()
//{
//	trainingDB->clearAll();
//}
//
//void DatabaseDialog::createTrainingDB()
//{
//	trainingDB->createLines(this);
//}

void DatabaseDialog::openComputerDB()
{
	QString path = QFileDialog::getOpenFileName(this, tr("Open computer evaluation book"), computerFile->text(), tr("Computer evaluation files (*.pcp)"));
	if (!path.isEmpty())
	{
		computerDB->close();
		computerFile->setText(path);
		if (!computerDB->open(path))
			computerDB->create(path);
	}
}

void DatabaseDialog::newComputerDB()
{
	QString path = QFileDialog::getSaveFileName(this, tr("New computer evaluation book"), computerFile->text(), tr("Computer evaluation files (*.pcp)"));
	if (!path.isEmpty())
	{
		computerDB->close();
		computerFile->setText(path);
		if (!computerDB->open(path))
			computerDB->create(path);
	}
}

void DatabaseDialog::closeComputerDB()
{
	computerFile->setText("");
}

void DatabaseDialog::openStatisticDB()
{
	QString path = QFileDialog::getOpenFileName(this, tr("Open statistics book"), statisticFile->text(), tr("Statistic files (*.pst)"));
	if (!path.isEmpty())
	{
		statDB->close();
		statisticFile->setText(path);
		if (!statDB->open(path))
			statDB->create(path);
	}
}

void DatabaseDialog::newStatisticDB()
{
	QString path = QFileDialog::getSaveFileName(this, tr("New statistics book"), statisticFile->text(), tr("Statistic files (*.pst)"));
	if (!path.isEmpty())
	{
		statDB->close();
		statisticFile->setText(path);
		if (!statDB->open(path))
			statDB->create(path);
	}
}

void DatabaseDialog::closeStatisticDB()
{
	statisticFile->setText("");
	statDB->close();
}

void DatabaseDialog::openOpeningsDB()
{
	QString path = QFileDialog::getOpenFileName(this, tr("Open openings book"), openingsFile->text(), tr("Openings files (*.pop)"));
	if (!path.isEmpty())
	{
		openingsDB->close();
		openingsFile->setText(path);
		if (!openingsDB->open(path))
			openingsDB->create(path);
	}
}

void DatabaseDialog::newOpeningsDB()
{
	QString path = QFileDialog::getSaveFileName(this, tr("New openings book"), openingsFile->text(), tr("Openings files (*.pop)"));
	if (!path.isEmpty())
	{
		openingsDB->close();
		openingsFile->setText(path);
		if (!openingsDB->open(path))
			openingsDB->create(path);
	}
}

void DatabaseDialog::closeOpeningsDB()
{
	openingsFile->setText("");
	openingsDB->close();
}

void DatabaseDialog::importBookTheory()
{
	Database iBase("ImportBase");
	QMessageBox msgbox;
	QString path = QFileDialog::getOpenFileName(this, "Open book to import", QString(), "Book files (*.pbk)");
	if (!path.isEmpty())
	{
		if (!iBase.open(path))
		{
			msgbox.setText("Can't open book to import");
			msgbox.exec();
			return;
		}
		theoryDB->importBase(&iBase);
	}
}

void DatabaseDialog::importBookWhite()
{
	Database iBase("ImportBase");
	QMessageBox msgbox;
	QString path = QFileDialog::getOpenFileName(this, "Open book to import", QString(), "Book files (*.pbk)");
	if (!path.isEmpty())
	{
		if (!iBase.open(path))
		{
			msgbox.setText("Can't open book to import");
			msgbox.exec();
			return;
		}
		whiteDB->importBase(&iBase);
	}
}

void DatabaseDialog::importBookBlack()
{
	Database iBase("ImportBase");
	QMessageBox msgbox;
	QString path = QFileDialog::getOpenFileName(this, "Open book to import", QString(), "Book files (*.pbk)");
	if (!path.isEmpty())
	{
		if (!iBase.open(path))
		{
			msgbox.setText("Can't open book to import");
			msgbox.exec();
			return;
		}
		blackDB->importBase(&iBase);
	}
}

void DatabaseDialog::importPGNTheory()
{
	if (!theoryDB->isOpen())
		return;
	bool variation = true;
	bool comment = true;
	int nummoves=999;
	QString path;
	ImportPgnDialog dialog(this);
	dialog.setItems(nummoves, comment, variation);
	if (dialog.exec() == QDialog::Rejected)
		return;
	dialog.getItems(path, nummoves, comment, variation);
	QFile file(path);
	if (!file.exists())
		return;
	dialog.importPgnFile(this, theoryDB, path, nummoves, comment, variation);
}

void DatabaseDialog::importPGNWhite()
{
	if (!whiteDB->isOpen())
		return;
	bool variation = true;
	bool comment = true;
	int nummoves = 999;
	QString path;
	ImportPgnDialog dialog(this);
	dialog.setItems(nummoves, comment, variation);
	if (dialog.exec() == QDialog::Rejected)
		return;
	dialog.getItems(path, nummoves, comment, variation);
	QFile file(path);
	if (!file.exists())
		return;
	dialog.importPgnFile(this, whiteDB, path, nummoves, comment, variation);
}

void DatabaseDialog::importPGNBlack()
{
	if (!blackDB->isOpen())
		return;
	bool variation = true;
	bool comment = true;
	int nummoves = 999;
	QString path;
	ImportPgnDialog dialog(this);
	dialog.setItems(nummoves, comment, variation);
	if (dialog.exec() == QDialog::Rejected)
		return;
	dialog.getItems(path, nummoves, comment, variation);
	QFile file(path);
	if (!file.exists())
		return;
	dialog.importPgnFile(this, blackDB, path, nummoves, comment, variation);
}

void DatabaseDialog::exportPGNTheory()
{
	if (!theoryDB->isOpen())
		return;
	ExportPgnDialog dialog(this);
	if (dialog.exec() == QDialog::Rejected)
		return;

	dialog.exportPgnFile(this, theoryDB);
}

void DatabaseDialog::exportPGNWhite()
{
	if (!whiteDB->isOpen())
		return;
	ExportPgnDialog dialog(this);
	if (dialog.exec() == QDialog::Rejected)
		return;

	dialog.exportPgnFile(this, whiteDB);
}

void DatabaseDialog::exportPGNBlack()
{
	if (!blackDB->isOpen())
		return;
	ExportPgnDialog dialog(this);
	if (dialog.exec() == QDialog::Rejected)
		return;

	dialog.exportPgnFile(this, blackDB);
}

void DatabaseDialog::importStatisticsDB()
{
	statDB->importGames(this);
}

void DatabaseDialog::compactStatisticsDB()
{
	statDB->removeSingleGame(this);
}

void DatabaseDialog::importOpeningsDB()
{
	openingsDB->importPgn(this);
}

void DatabaseDialog::exportOpeningsDB()
{
	openingsDB->exportPgn(this);
}

