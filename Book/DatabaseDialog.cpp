#include "DatabaseDialog.h"
#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QPushButton>
#include <QFileDialog>
#include <QGridLayout>
#include <QSpacerItem>
#include <QMessageBox>

DatabaseDialog::DatabaseDialog(QWidget* parent, Database* theory, Database* white, Database* black, Statistics* stat)
	:QDialog(parent)
{
	statDB = stat;
	theoryDB = theory;
	whiteDB = white;
	blackDB = black;
	QVBoxLayout* vbox;
	QGroupBox* group;
	QPushButton* button;
	QGridLayout* grid;

	vbox = new QVBoxLayout;

	group = new QGroupBox(tr("Theory book"));
	grid = new QGridLayout();
	theoryFile = new QLineEdit;
	theoryFile->setDisabled(true);
	grid->addWidget(theoryFile, 0, 0, 1, -1);
	button = new QPushButton(tr("Open"));
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
	grid->setAlignment(Qt::AlignLeft);
	group->setLayout(grid);
	vbox->addWidget(group);

	group = new QGroupBox(tr("White repertoire book"));
	whiteFile = new QLineEdit;
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
	grid->setAlignment(Qt::AlignLeft);
	group->setLayout(grid);
	vbox->addWidget(group);

	group = new QGroupBox(tr("Black repertoire book"));
	grid = new QGridLayout();
	blackFile = new QLineEdit;
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
	grid->setAlignment(Qt::AlignLeft);
	group->setLayout(grid);
	vbox->addWidget(group);

	group = new QGroupBox(tr("Training statistics"));
	grid = new QGridLayout();
	trainingFile = new QLineEdit;
	trainingFile->setDisabled(true);
	grid->addWidget(trainingFile, 0, 0, 1, -1);
	button = new QPushButton(tr("Open"));
	connect(button, SIGNAL(clicked()), this, SLOT(openTrainingDB()));
	grid->addWidget(button, 1, 0);
	button = new QPushButton(tr("New"));
	connect(button, SIGNAL(clicked()), this, SLOT(newTrainingDB()));
	grid->addWidget(button, 1, 1);
	button = new QPushButton(tr("Close"));
	connect(button, SIGNAL(clicked()), this, SLOT(closeTrainingDB()));
	grid->addWidget(button, 1, 2);
	QSize size = button->sizeHint();
	QSpacerItem* spacer = new QSpacerItem(size.width(), size.height());
	grid->addItem(spacer, 1, 3);
	spacer = new QSpacerItem(size.width(), size.height());
	grid->addItem(spacer, 1, 4);
	grid->setAlignment(Qt::AlignLeft);
	group->setLayout(grid);
	vbox->addWidget(group);

	group = new QGroupBox(tr("Computer evaluation"));
	grid = new QGridLayout();
	computerFile = new QLineEdit;
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
	spacer = new QSpacerItem(size.width(), size.height());
	grid->addItem(spacer, 1, 3);
	spacer = new QSpacerItem(size.width(), size.height());
	grid->addItem(spacer, 1, 4);
	grid->setAlignment(Qt::AlignLeft);
	group->setLayout(grid);
	vbox->addWidget(group);

	group = new QGroupBox(tr("Games statistic"));
	grid = new QGridLayout();
	statisticFile = new QLineEdit;
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
	connect(button, SIGNAL(clicked()), this, SLOT(importStatisticDB()));
	button->setToolTip(tr("Import pgn file into the statistics."));
	grid->addWidget(button, 1, 3);
	button = new QPushButton(tr("Compact"));
	connect(button, SIGNAL(clicked()), this, SLOT(compactStatisticDB()));
	button->setToolTip(tr("Remove positions with just a single game."));
	grid->addWidget(button, 1, 4);
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
		theoryFile->setText(path);
		if (!theoryDB->open(path))
			theoryDB->create(path, 0);
	}
}

void DatabaseDialog::newTheoryDB()
{
	QString path = QFileDialog::getSaveFileName(this, tr("New theory book"), theoryFile->text(), tr("Book files (*.pbk)"));
	if (!path.isEmpty())
	{
		theoryFile->setText(path);
		if (!theoryDB->open(path))
			theoryDB->create(path, 0);
	}
}

void DatabaseDialog::openWhiteDB()
{
	QString path = QFileDialog::getOpenFileName(this, tr("Open White repertoire book"), whiteFile->text(), tr("Book files (*.pbk)"));
	if (!path.isEmpty())
	{
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
		whiteFile->setText(path);
		if (!whiteDB->open(path))
			whiteDB->create(path, 1);
	}
}

void DatabaseDialog::openBlackDB()
{
	QString path = QFileDialog::getOpenFileName(this, tr("Open Black repertoire book"), blackFile->text(), tr("Book files (*.pbk)"));
	if (!path.isEmpty())
	{
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
		blackFile->setText(path);
		if (!blackDB->open(path))
			blackDB->create(path, 2);
	}
}

void DatabaseDialog::openTrainingDB()
{
	QString path = QFileDialog::getOpenFileName(this, tr("Open training book"), trainingFile->text(), tr("Training files (*.ptr)"));
	if (!path.isEmpty())
		trainingFile->setText(path);
}

void DatabaseDialog::newTrainingDB()
{
	QString path = QFileDialog::getSaveFileName(this, tr("New training book"), trainingFile->text(), tr("Training files (*.ptr)"));
	if (!path.isEmpty())
		trainingFile->setText(path);
}

void DatabaseDialog::openComputerDB()
{
	QString path = QFileDialog::getOpenFileName(this, tr("Open computer evaluation book"), computerFile->text(), tr("Computer evaluation files (*.pcp)"));
	if (!path.isEmpty())
		computerFile->setText(path);
}

void DatabaseDialog::newComputerDB()
{
	QString path = QFileDialog::getSaveFileName(this, tr("New computer evaluation book"), computerFile->text(), tr("Computer evaluation files (*.pcp)"));
	if (!path.isEmpty())
		computerFile->setText(path);
}

void DatabaseDialog::openStatisticDB()
{
	QString path = QFileDialog::getOpenFileName(this, tr("Open statistics book"), statisticFile->text(), tr("Statistic files (*.pst)"));
	if (!path.isEmpty())
		statisticFile->setText(path);
}

void DatabaseDialog::newStatisticDB()
{
	QString path = QFileDialog::getSaveFileName(this, tr("New statistics book"), statisticFile->text(), tr("Statistic files (*.pst)"));
	if (!path.isEmpty())
		statisticFile->setText(path);
}

void DatabaseDialog::closeTheoryDB()
{
	theoryFile->setText("");
	theoryDB->close();
}

void DatabaseDialog::closeWhiteDB()
{
	whiteFile->setText("");
	whiteDB->close();
}

void DatabaseDialog::closeBlackDB()
{
	blackFile->setText("");
	blackDB->close();
}

void DatabaseDialog::closeTrainingDB()
{
	trainingFile->setText("");
}

void DatabaseDialog::closeComputerDB()
{
	computerFile->setText("");
}

void DatabaseDialog::closeStatisticDB()
{
	statisticFile->setText("");
	statDB->close();
}

void DatabaseDialog::getItems(QString& theory, QString& white, QString& black, QString& training, QString& computer, QString& statistic)
{
	theory = theoryFile->text();
	white = whiteFile->text();
	black = blackFile->text();
	training = trainingFile->text();
	computer = computerFile->text();
	statistic = statisticFile->text();
}

void DatabaseDialog::setItems(const QString& theory, const QString& white, const QString& black, const QString& training, const QString& computer, const QString& statistic)
{
	theoryFile->setText(theory);
	whiteFile->setText(white);
	blackFile->setText(black);
	trainingFile->setText(training);
	computerFile->setText(computer);
	statisticFile->setText(statistic);
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
}

void DatabaseDialog::importPGNWhite()
{
}

void DatabaseDialog::importPGNBlack()
{
}

void DatabaseDialog::importStatistics()
{
	statDB->importGames(this);
}

void DatabaseDialog::compactStatistics()
{
	statDB->removeSingleGame(this);
}

