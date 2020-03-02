#include "TrainingWindow.h"
#include "TrainingDialog.h"
#include <QMenu>
#include <QFontDialog>
#include <QBoxLayout>
#include <QPushButton>
#include <QTextStream>
#include <QApplication>

TrainingWindow::TrainingWindow(QWidget* parent)
	: QWidget(parent)
{
	running = false;
	Base[0] = Base[1] = Base[2] = NULL;
	theoryColor.setRgb(0, 0, 128);
	repColor.setRgb(0, 128, 0);

	QPushButton* button;
	QVBoxLayout* vbox;
	QHBoxLayout* hbox;

	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));

	vbox = new QVBoxLayout;

	hbox = new QHBoxLayout;
	watch = new StatusWatch;
	rowid = new QLabel(tr("Id: "));
	lRunning = new QLabel(tr("Not running"));
	inBase = new QLabel(tr("In base: "));
	loaded = new QLabel(tr("Loaded: "));
	score = new QLabel(tr("Score: "));
	hbox->addWidget(rowid);
	hbox->addWidget(lRunning);
	hbox->addWidget(watch);
	hbox->addWidget(inBase);
	hbox->addWidget(loaded);
	hbox->addWidget(score);
	vbox->addLayout(hbox);

	hbox = new QHBoxLayout;
	comment = new QTextEdit;
	comment->setDisabled(true);
	hbox->addWidget(comment);
	vbox->addLayout(hbox);

	hbox = new QHBoxLayout;
	colorBox = new QComboBox;
	colorBox->addItem(tr("Both"));
	colorBox->addItem(tr("White"));
	colorBox->addItem(tr("Black"));
	colorBox->setFixedWidth(colorBox->minimumSizeHint().width());
	hbox->addWidget(colorBox);
	positionBox = new QCheckBox(tr("From current position"));
	hbox->addWidget(positionBox);
	vbox->addLayout(hbox);

	hbox = new QHBoxLayout;
	button = new QPushButton(tr("Stop training"));
	connect(button, SIGNAL(clicked()), this, SLOT(next()));
	button->setFixedWidth(button->minimumSizeHint().width());
	hbox->addWidget(button);
	button = new QPushButton(tr("Next"));
	connect(button, SIGNAL(clicked()), this, SLOT(next()));
	button->setFixedWidth(button->minimumSizeHint().width());
	hbox->addWidget(button);

	vbox->addLayout(hbox);
	vbox->setAlignment(Qt::AlignRight);
	setLayout(vbox);
}

TrainingWindow::~TrainingWindow()
{
}

void TrainingWindow::showContextMenu(const QPoint& pos)
{
	QMenu* contextMenu = new QMenu(this);
	contextMenu->addAction(QString("Font"), this, SLOT(selectFont()));
	contextMenu->exec(mapToGlobal(pos));
}

void TrainingWindow::selectFont()
{
	bool ok;
	QFont f = QFontDialog::getFont(&ok, font(), this);
	if (ok)
	{
		setFont(f);
	}
}

QString TrainingWindow::fontToString()
{
	return font().toString();
}

void TrainingWindow::fontFromString(const QString& sFont)
{
	if (sFont.isEmpty())
		return;
	QFont f;
	f.fromString(sFont);
	setFont(f);
}

void TrainingWindow::next()
{
	if (running)
		return;
	running = false;
	updateStat();
	updateComment(false);
	ChessBoard cb, b;
	int i;
	int color = colorBox->currentIndex()-1;
	if (positionBox->isChecked())
		cb = currentBoard;
	else
		cb.setStartposition();
	if (!trainingDB->getNext(trainingLine, color, cb))
	{
		updateStat();
		return;
	}
	emit trainingFlipBoard(trainingLine.color);
	
	// Go to startposition of this training line
	trainingLine.current = 0;
	b.setStartposition();
	for (i = 0; i < trainingLine.moves.size(); i++)
	{
		if (cb == b)
			break;
		b.doMove(trainingLine.moves[i].move,false);
		++trainingLine.current;
	}

	// Startposition not found
	if (i == trainingLine.moves.size())
		return;

	// If wrong color to move go one move forward
	if (b.toMove != trainingLine.color)
	{
		ChessMove move = trainingLine.currentMove();
		emit trainingSetArrow(SQUARE64(move.fromSquare), SQUARE64(move.toSquare), false, 10);
		++trainingLine.current;
	}
	MoveList* ml = new MoveList;
	trainingLine.moveList(*ml);
	emit trainingAddMoves(*ml);
	running = true;
	updateStat();
	delete ml;
}

void TrainingWindow::setCurrentBoard(const ChessBoard& cb)
{
	currentBoard = cb;
}

void TrainingWindow::setTrainingDB(Training* db)
{
	trainingDB = db;
}

void TrainingWindow::updateStat()
{
	TrainingStatistics ts = trainingDB->getStat();
	QString qs = tr("In base: ");
	QTextStream(&qs) << ts.inBase;
	inBase->setText(qs);
	qs = tr("Id: ");
	QTextStream(&qs) << trainingLine.rowid;
	rowid->setText(qs);
	qs = tr("Loaded: ");
	QTextStream(&qs) << ts.inBase;
	loaded->setText(qs);
	qs = tr("Score: ");
	QTextStream(&qs) << trainingLine.score;
	score->setText(qs);
	if (running)
		lRunning->setText(tr("Running"));
	else
		lRunning->setText(tr("Not running"));
}

void TrainingWindow::moveEntered(ChessMove& move)
{
	MoveList* ml= new MoveList;
	ChessMove nextmove;
	int attempt = trainingLine.attempt();
	trainingLine.incAttempt();
	if (trainingLine.isCorrect(move))
	{
		if (!attempt)
			trainingLine.incScore();
		if (trainingLine.nextMove(nextmove))
		{
			updateComment(false);
			trainingLine.moveList(*ml);
			emit trainingAddMoves(*ml);
			emit trainingSetArrow(SQUARE64(nextmove.fromSquare), SQUARE64(nextmove.toSquare), false, 10);
			updateStat();
		}
		else
		{
			running = false;
			++trainingLine.current;
			trainingLine.moveList(*ml);
			emit trainingAddMoves(*ml);
			QApplication::processEvents();
			trainingDB->updateScore(trainingLine);
			updateComment(true);
			updateStat();
			TrainingDialog dialog(this, QString(), QString(), QString());
			switch (dialog.exec())
			{
			case 1:
				break;
			case 2:
				break;
			case 3:
				break;
			}
		}
		return;
	}
	updateComment(true);
	nextmove = trainingLine.currentMove();
	trainingLine.moveList(*ml);
	emit trainingAddMoves(*ml);
	emit trainingSetArrow(SQUARE64(nextmove.fromSquare), SQUARE64(nextmove.toSquare), true, 10);
//	trainingLine.decScore();
	updateStat();
	delete ml;
}

void TrainingWindow::stopRunning()
{
	running = false;
}

void TrainingWindow::setDatabase(Database* t, Database* w, Database* b)
{
	Base[0] = t;
	Base[1] = w;
	Base[2] = b;
}

void TrainingWindow::updateComment(bool visible)
{
	BookDBEntry dbe;
	ChessBoard cb=trainingLine.currentPosition();
	if (!visible)
	{
		comment->hide();
		return;
	}
	comment->show();
	comment->clear();
	dbe = Base[0]->find(cb);
	if (!dbe.comment.isEmpty())
	{
		comment->setTextColor(theoryColor);
		comment->append(dbe.comment);
	}
	dbe = Base[1]->find(cb);
	if (!dbe.comment.isEmpty())
	{
		comment->setTextColor(repColor);
		comment->append(dbe.comment);
	}
	dbe = Base[2]->find(cb);
	if (!dbe.comment.isEmpty())
	{
		comment->setTextColor(repColor);
		comment->append(dbe.comment);
	}
}