#include "ImportPgnDialog.h"
#include "Database.h"
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
		if (white)
			whiterep->setChecked(true);
		else
			blackrep->setChecked(true);
	}

	char sz[16];
	numberofmoves->setText(itoa(moves, sz, 10));
}

void ImportPgnDialog::getItems(QString& path, bool& theory, bool& rep, bool& white, int& moves, bool& com, bool& var)
{
	path=filename->text();
	theory = theoryfile->isChecked();
	rep = repfile->isChecked();
	white = whiterep->isChecked();
	com = comment->isChecked();
	var = variation->isChecked();

	moves=numberofmoves->text().toInt();
}

void ImportPgnDialog::openFile()
{
	QString path = QFileDialog::getOpenFileName(this, "Open pgnfile", QString(), "Pgn files (*.pgn)");
	if (!path.isEmpty())
		filename->setText(path);

}

void ImportPgnDialog::typeChanged()
{
	if (theoryfile->isChecked())
	{
		blackrep->setEnabled(false);
		whiterep->setEnabled(false);
	}
	else
	{
		blackrep->setEnabled(true);
		whiterep->setEnabled(true);
	}
}

void ImportPgnDialog::importPgnFile(QWidget* parent, Database* db, QString& pgnfile, bool whiterep, bool blackrep, int moves, bool comment, bool variation)
{
	QProgressDialog progress("Importing Pgn file: " + pgnfile, "Cancel", 0, 100, this);
	progress.setMinimumDuration(0);
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
	int i, j, k, l;
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
				if (comment && !game.position[j].comment.empty())
				{
					if (!bde.comment.isEmpty())
						bde.comment = "\n";
					bde.comment += game.position[j].comment.c_str();
				}
				for (k = 0; k < game.position[j].move.size(); k++)
				{
					bdm.clear();
					bdm.move = game.position[j].move[k].move;
					if (!bde.moveExist(bdm.move))
					{
						if (comment && !game.position[j].move[k].comment.empty())
							bdm.comment = game.position[j].move[k].comment.c_str();
						if (whiterep)
							bdm.whiterep = 1;
						else if (blackrep)
							bdm.blackrep = 1;
						bde.movelist.push_back(bdm);
					}
					else
					{
						for (l = 0; l < bde.movelist.size(); l++)
						{
							if (bdm.move == bde.movelist[l].move)
							{
								if (comment && !game.position[j].move[k].comment.empty())
								{
									if (bde.movelist[l].comment.isEmpty())
										bde.movelist[l].comment = game.position[j].move[k].comment.c_str();
								}
								if (whiterep)
									bde.movelist[l].whiterep = 1;
								else if (blackrep)
									bde.movelist[l].blackrep = 1;
								break;
							}
						}
					}

				}
				db->add(bde);
			}
		}
	}
	progress.setValue(games);
	pgn.close();
}
