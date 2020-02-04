#include "ExportPgnDialog.h"
#include "Database.h"
#include <algorithm>
#include <QBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFile>

ExportPgnDialog::ExportPgnDialog(QWidget *parent)
	: QDialog(parent)
{
	QVBoxLayout* vbox;
	QHBoxLayout* hbox;
	QPushButton* button;
	QButtonGroup* group;
	QLabel* label;
	vbox = new QVBoxLayout;
	setWindowTitle(tr("Export to pgn file"));

	hbox = new QHBoxLayout;
	filename = new QLineEdit;
	button = new QPushButton("...");
	hbox->addWidget(filename);
	hbox->addWidget(button);
	connect(button, SIGNAL(clicked()), this, SLOT(saveFile()));
	vbox->addLayout(hbox);

	QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
	connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
	vbox->addWidget(buttons);

	setLayout(vbox);
}

void ExportPgnDialog::saveFile()
{
	QString path = QFileDialog::getSaveFileName(this, "Save pgn file", QString(), "Pgn files (*.pgn)");
	if (!path.isEmpty())
		filename->setText(path);
}

void ExportPgnDialog::exportPgnFile(QWidget* parent, Database* db)
{
	QVector<BookDBEntry> pos;
	QString path=filename->text();
	QFile file(path);
	if (file.exists())
		file.remove();
	Pgn pgn;
	ChessGame game;
	pgn.open(path.toStdString(), false);

	db->getTrainingPosition(pos);
	std::sort(pos.begin(), pos.end());

	ChessBoard cb;
	cb.setStartposition();
	game.clear();
	game.setStartPosition(cb);
	moves.clear();
	walkThrough(cb, 0, pos, pgn);
	pgn.close();
}

void ExportPgnDialog::walkThrough(ChessBoard& cb, int ply, QVector<BookDBEntry>& pos, Pgn& pgn)
{
	int  curmove = 0;
	BookDBEntry bde;
	ExportMove em;
	QVector<BookDBEntry>::iterator bid;
	//TrainingDBMove tpe;

	//// Get position 
	bde.board = cb;
	bid = std::lower_bound(pos.begin(), pos.end(), bde);

	//// The position don't exist or no moves or repeating move
	if ((bid->board != cb) || (bid->movelist.size() == 0) || (bid->dirty))
	{
		ChessGame game;
		ChessBoard b;
		b.setStartposition();
		game.setStartPosition(b);
		for (int i = 0; i < moves.size(); i++)
		{
			game.addBoardComment(moves[i].comment.toStdString());
			game.addMove(moves[i].move);
			game.addMoveComment(moves[i].moveComment.toStdString());
			moves[i].comment.clear();
			moves[i].moveComment.clear();
		}
		// Add comment for end position
		if (!bid->dirty && (bid->board == cb))
			game.addBoardComment(bid->comment.toStdString());
		pgn.appendGame(game);
		return;
	}

	bid->dirty = true;
	em.comment = bid->comment;
	while (1)
	{
		em.moveComment = bid->movelist[curmove].comment;
		em.move=bid->movelist[curmove].move;
		moves.push_back(em);
		cb.doMove(bid->movelist[curmove].move, false);
		walkThrough(cb, ply + 1, pos, pgn);
		em.comment.clear();
		moves.pop_back();
		++curmove;
		if (curmove >= bid->movelist.size())
			break;
		cb = bde.board;
	}
}
