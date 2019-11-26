#include "CreateStatisticsDialog.h"
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QRadioButton>
#include <QCheckBox>
#include <QLabel>
#include <QButtonGroup>
#include <QMessageBox>
#include <QProgressDialog>
#include "../Common/Pgn.h"
#include "../Common/ChessGame.h"

CreateStatisticsDialog::CreateStatisticsDialog(QWidget *parent)
	: QDialog(parent)
{
	/*
	QVBoxLayout* vbox;
	QHBoxLayout* hbox;
	QPushButton* button;
	QButtonGroup* group;
	QLabel* label;
	vbox = new QVBoxLayout;
	setWindowTitle(tr("Import pgnfiles"));

	hbox = new QHBoxLayout;
	filename = new QLineEdit;
	button = new QPushButton("...");
	hbox->addWidget(filename);
	hbox->addWidget(button);
	connect(button, SIGNAL(clicked()), this, SLOT(openFile()));
	vbox->addLayout(hbox);

	hbox = new QHBoxLayout;
	label = new QLabel("Filetype:");
	theoryfile = new QRadioButton("Theory");
	repfile = new QRadioButton("Repertoire");
	group = new QButtonGroup;
	group->addButton(theoryfile);
	group->addButton(repfile);
	hbox->addWidget(label);
	hbox->addWidget(theoryfile);
	hbox->addWidget(repfile);
	vbox->addLayout(hbox);
	theoryfile->setEnabled(false);
	repfile->setEnabled(false);
	connect(theoryfile, SIGNAL(clicked()), this, SLOT(typeChanged()));
	connect(repfile, SIGNAL(clicked()), this, SLOT(typeChanged()));

	hbox = new QHBoxLayout;
	label = new QLabel("Repertoire:");
	whiterep = new QRadioButton("White");
	blackrep = new QRadioButton("Black");
	group = new QButtonGroup;
	group->addButton(whiterep);
	group->addButton(blackrep);
	hbox->addWidget(label);
	hbox->addWidget(whiterep);
	hbox->addWidget(blackrep);
	vbox->addLayout(hbox);
	whiterep->setChecked(true);
	whiterep->setEnabled(false);
	blackrep->setEnabled(false);

	hbox = new QHBoxLayout;
	comment = new QCheckBox("Include comment");
	variation = new QCheckBox("Include varitions");
	hbox->addWidget(comment);
	hbox->addWidget(variation);
	vbox->addLayout(hbox);
	comment->setChecked(true);
	variation->setChecked(true);

	hbox = new QHBoxLayout;
	label = new QLabel("Game length:");
	numberofmoves = new QLineEdit;
	hbox->addWidget(label);
	hbox->addWidget(numberofmoves);
	vbox->addLayout(hbox);

	hbox = new QHBoxLayout;
	button = new QPushButton("Cancel");
	hbox->addWidget(button);
	connect(button, SIGNAL(clicked()), this, SLOT(reject()));
	button = new QPushButton("Ok");
	hbox->addWidget(button);
	connect(button, SIGNAL(clicked()), this, SLOT(accept()));
	vbox->addLayout(hbox);

	setLayout(vbox);
	*/
}

