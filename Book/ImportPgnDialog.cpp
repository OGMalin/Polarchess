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
#include <QApplication>
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

void ImportPgnDialog::setItems(int moves, bool com, bool var)
{
	if (var)
		variation->setChecked(true);
	else
		variation->setChecked(false);
	if (com)
		comment->setChecked(true);
	else
		comment->setChecked(false);
	char sz[16];
	numberofmoves->setText(itoa(moves, sz, 10));
}

void ImportPgnDialog::getItems(QString& path, int& moves, bool& com, bool& var)
{
	path=filename->text();
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

void ImportPgnDialog::importPgnFile(QWidget* parent, Database* db, QString& pgnfile, int moves, bool comment, bool variation)
{
	QProgressDialog progress("Importing Pgn file: " + pgnfile, "Cancel", 0, 100, this);
	progress.setWindowModality(Qt::WindowModal);
	progress.setMinimumDuration(0);
	progress.show();
	Pgn pgn;
	if (!pgn.open(pgnfile.toStdString(), true))
	{
		QMessageBox msgBox(parent);
		msgBox.setText("Can't open pgn file: " + pgnfile);
		progress.setValue(100);
		return;
	}
	int games = pgn.size();
	progress.setMaximum(games);
	if (games < 1)
	{
		QMessageBox msgBox(parent);
		msgBox.setText("No games in the pgn file: " + pgnfile);
		pgn.close();
		progress.setValue(games);
		return;
	}
	ChessGame game;
	BookDBEntry bde;
	BookDBMove bdm;
	int i, j, k, l;

	for (i = 0; i < games; i++)
	{
		progress.setValue(i);
		QApplication::processEvents();
		if (progress.wasCanceled())
		{
			pgn.close();
			return;
		}
		if (pgn.read(game, i + 1, moves))
		{
			for (j = 0; j < game.position.size(); j++)
			{
				bde = db->find(game.position[j].board);
				if (comment && !game.position[j].comment.empty())
				{
					if (!bde.comment.isEmpty())
						bde.comment = "\n";
					bde.comment += QString::fromLatin1(game.position[j].comment.c_str());
				}
				for (k = 0; k < game.position[j].move.size(); k++)
				{
					bdm.clear();
					bdm.move = game.position[j].move[k].move;
					if (!bde.moveExist(bdm.move))
					{
						if (comment && !game.position[j].move[k].comment.empty())
							bdm.comment = QString::fromLatin1(game.position[j].move[k].comment.c_str());
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
										bde.movelist[l].comment = QString::fromLatin1(game.position[j].move[k].comment.c_str());
								}
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
	QApplication::processEvents();
	pgn.close();
}
