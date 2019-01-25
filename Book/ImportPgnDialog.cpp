#include "ImportPgnDialog.h"
#include "Database.h"
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QRadioButton>
#include <QLabel>
#include <QButtonGroup>
#include <QMessageBox>
#include <QProgressDialog>
#include "../Common/Pgn.h"
#include "../Common/ChessGame.h"

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

void ImportPgnDialog::importPgnFile(QWidget* parent, Database* db, QString& pgnfile, bool whiterep, bool blackrep, int moves)
{
	QProgressDialog progress("Importing Pgn file: " + pgnfile, "Cancel", 0, 0, this);
	progress.setWindowModality(Qt::WindowModal);
	Pgn pgn;
	if (!pgn.open(pgnfile.toStdString(), true))
	{
		QMessageBox msgBox(parent);
		msgBox.setText("Can't open pgn file: " + pgnfile);
		return;
	}
	int games = pgn.size();
	progress.setMaximum(games);
	if (games < 1)
	{
		QMessageBox msgBox(parent);
		msgBox.setText("No games in the pgn file: " + pgnfile);
		pgn.close();
		return;
	}
	ChessGame game;
	BookDBEntry bde;
	BookDBMove bdm;
	int i, j, k;
	for (i = 0; i < games; i++)
	{
		progress.setValue(i);
		if (progress.wasCanceled())
		{
			pgn.close();
			return;
		}
		if (pgn.read(game, i + 1, moves))
		{
			for (j = 0; j < game.position.size(); j++)
			{
				if (progress.wasCanceled())
				{
					pgn.close();
					return;
				}
				bde=db->find(game.position[j].board);
				bde.board=game.position[j].board;
				for (k = 0; k < game.position[j].move.size(); k++)
				{
					bdm.clear();
					bdm.move = game.position[j].move[k].move;
					bdm.comment = game.position[j].move[k].comment.c_str();
					bde.movelist.push_back(bdm);
				}
			}
			db->add(bde);
		}
	}
	progress.setValue(games);
	pgn.close();
}
