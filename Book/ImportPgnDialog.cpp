#include "ImportPgnDialog.h"
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QRadioButton>
#include <QLabel>
#include <QButtonGroup>

ImportPgnDialog::ImportPgnDialog(QWidget *parent)
	: QDialog(parent)
{
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
	theoryfile= new QRadioButton("Theory");
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
	whiterep->setEnabled(false);
	blackrep->setEnabled(false);

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
}

void ImportPgnDialog::setItems(bool theory, bool rep, bool white, int moves)
{
	if (theory)
	{
		theoryfile->setEnabled(true);
		theoryfile->setChecked(true);
	}
	if (rep)
	{
		repfile->setEnabled(true);
		whiterep->setEnabled(true);
		blackrep->setEnabled(true);
		if (!theory)
			repfile->setChecked(true);
	}
	char sz[16];
	numberofmoves->setText(itoa(moves, sz, 10));
}

void ImportPgnDialog::getItems(QString& path, bool& theory, bool& rep, bool& white, int& moves)
{
	path=filename->text();
	if (theoryfile->isChecked())
		theory = true;
	if (repfile->isChecked())
		rep = true;
	if (whiterep->isChecked())
		white = true;
	else
		white = false;
	moves=numberofmoves->text().toInt();
}

void ImportPgnDialog::openFile()
{
	QString path = QFileDialog::getOpenFileName(this, "Open pgnfile", QString(), "Pgn files (*.pgn)");
	if (!path.isEmpty())
		filename->setText(path);

}