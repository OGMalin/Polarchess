#include "TrainingWindow.h"
#include "TrainingDialog.h"
#include <QMenu>
#include <QFontDialog>
#include <QBoxLayout>
#include <QTextStream>
#include <QApplication>

TrainingWindow::TrainingWindow(QWidget* parent)
	: QWidget(parent)
{
	running = false;
	Base[0] = Base[1] = Base[2] = NULL;
	compDB = NULL;
	theoryColor.setRgb(0, 0, 128);
	repColor.setRgb(0, 128, 0);
	trainingLine.clear();
	QVBoxLayout* vbox;
	QHBoxLayout* hbox;

	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));

	vbox = new QVBoxLayout;

	hbox = new QHBoxLayout;
	watch = new Watch(NULL,true);
//	rowid = new QLabel(tr("Id: "));
	lRunning = new QLabel(tr("Not running"));
	inBase = new QLabel(tr("In base: "));
	loaded = new QLabel(tr("Loaded: "));
	score = new QLabel(tr("Score: "));
//	hbox->addWidget(rowid);
	hbox->addWidget(lRunning);
	hbox->addWidget(watch);
	hbox->addWidget(inBase);
	hbox->addWidget(loaded);
	hbox->addWidget(score);
	vbox->addLayout(hbox);

	hbox = new QHBoxLayout;
	comment = new QTextEdit;
	comment->setReadOnly(true);
	hbox->addWidget(comment);
	vbox->addLayout(hbox);

	hbox = new QHBoxLayout;
	computerScore = new QLabel(tr("Computer score: "));
	hbox->addWidget(computerScore);
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
	movesBox = new QLineEdit;
	movesBox->setText("0");
	movesBox->setInputMask("9");
	hbox->addWidget(movesBox);
	vbox->addLayout(hbox);

	hbox = new QHBoxLayout;
	startButton = new QPushButton(tr("Start"));
	connect(startButton, SIGNAL(clicked()), this, SLOT(start()));
	startButton->setFixedWidth(startButton->minimumSizeHint().width());
	hbox->addWidget(startButton);

	vbox->addLayout(hbox);
	vbox->setAlignment(Qt::AlignRight);
	setLayout(vbox);

	connect(this, SIGNAL(nextResponse()), this, SLOT(next()));
	connect(this, SIGNAL(restart()), this, SLOT(start()));
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

void TrainingWindow::start()
{
	if (running)
		return;
	running = false;
	updateStat();
	updateComment(false);
	ChessBoard cb, b;
	int i;
	int color = colorBox->currentIndex()-1;
	int moves = movesBox->text().toInt();
	if (positionBox->isChecked())
		cb = currentBoard;
	else
		cb.setStartposition();
	if (!training->createLines(this, color, cb, moves))
	{
		updateStat();
		return;
	}

	updateStat();
	next();
}

void TrainingWindow::next()
{
	ChessBoard cb;
	int i;
	updateStat();
	if (!training->getNextLine(trainingLine))
	{
		running = false;
		updateStat();
		emit restart();
		return;
	}

	running = true;

	emit trainingFlipBoard(trainingLine.color);
		
	// Go to startposition of this training line
	trainingLine.current = 0;
	cb.setStartposition();
	for (i = 0; i < trainingLine.moves.size(); i++)
	{
		if (currentBoard == cb)
			break;
		cb.doMove(trainingLine.moves[i].move,false);
		++trainingLine.current;
	}
	
	// Startposition not found
	if (i == trainingLine.moves.size())
	{
		running = false;
		updateStat();
		return;
	}
	
	// If wrong color to move go one move forward
	if (cb.toMove != trainingLine.color)
	{
		ChessMove move = trainingLine.currentMove();
		emit trainingSetArrow(SQUARE64(move.fromSquare), SQUARE64(move.toSquare), false, 10);
		++trainingLine.current;
	}
	
	MoveList* ml = new MoveList;
	trainingLine.moveList(*ml);
	emit trainingAddMoves(*ml);
	delete ml;
	updateStat(1);
}

void TrainingWindow::setCurrentBoard(const ChessBoard& cb)
{
	currentBoard = cb;
}

void TrainingWindow::setTraining(Training* t)
{
	training = t;
}

void TrainingWindow::updateStat(int cur)
{
	TrainingStatistics ts = training->getStat();
	QString qs = tr("In base: ");
	QTextStream(&qs) << ts.inBase;
	inBase->setText(qs);
	qs = tr("Loaded: ");
	QTextStream(&qs) << (ts.current + cur);
	loaded->setText(qs);
	qs = tr("Score: ");
	QTextStream(&qs) << trainingLine.score;
	score->setText(qs);
	if (running)
	{
		lRunning->setText(tr("Running"));
		colorBox->setDisabled(true);
		positionBox->setDisabled(true);
		startButton->setDisabled(true);
	}
	else
	{
		lRunning->setText(tr("Not running"));
		lRunning->setText(tr("Running"));
		colorBox->setDisabled(false);
		positionBox->setDisabled(false);
		startButton->setDisabled(false);
	}
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
		}
		else
		{
			++trainingLine.current;
			trainingLine.moveList(*ml);
			emit trainingAddMoves(*ml);
			QApplication::processEvents();
			training->updateScore(trainingLine);
			updateComment(true);
			updateStat();
			TrainingDialog dialog(this, QString(), QString(), QString());
			switch (dialog.exec())
			{
			case 1: // Next
				emit nextResponse();
				break;
			case 2: // Stop
				running = false;
				break;
			case 3: // Analyze
				emit trainingStop();
				running = false;
				break;
			}
		}
//		updateStat();
		return;
	}
	updateComment(true);
	nextmove = trainingLine.currentMove();
	trainingLine.moveList(*ml);
	emit trainingAddMoves(*ml);
	emit trainingSetArrow(SQUARE64(nextmove.fromSquare), SQUARE64(nextmove.toSquare), true, 10);
//	updateStat();
	delete ml;
}

void TrainingWindow::stopRunning()
{
	running = false;
}

void TrainingWindow::setDatabase(Database* t, Database* w, Database* b, Computer* c)
{
	Base[0] = t;
	Base[1] = w;
	Base[2] = b;
	compDB = c;
}

void TrainingWindow::updateComment(bool visible)
{
	BookDBEntry bde;
	ComputerDBEntry cde;
	int score;
	int i, j;
	QString qs;
	ChessBoard cb=trainingLine.currentPosition();
	if (!visible)
	{
		comment->clear();
		qs = tr("Computer score: ");
		computerScore->setText(qs);
		return;
	}
	comment->clear();
	bde = Base[0]->find(cb);
	if (!bde.comment.isEmpty())
	{
		comment->setTextColor(theoryColor);
		comment->append(bde.comment);
	}
	bde = Base[1]->find(cb);
	if (!bde.comment.isEmpty())
	{
		comment->setTextColor(repColor);
		comment->append(bde.comment);
	}
	bde = Base[2]->find(cb);
	if (!bde.comment.isEmpty())
	{
		comment->setTextColor(repColor);
		comment->append(bde.comment);
	}
	cde = compDB->find(cb);
	qs = tr("Computer score: ");
	bool found = false;
	score = 0;
	if (!cde.cboard.isEmpty())
	{
		for (i = 0; i < compDB->enginelist.size(); i++)
		{
			for (j = 0; j < cde.enginelist.size(); j++)
			{
				if (compDB->enginelist[i] == cde.enginelist[j].engine)
				{
					score = cde.enginelist[j].cp;
					found = true;
					break;
				}
			}
			if (found)
				break;
		}
		if (found)
			QTextStream(&qs) << ((score > 0) ? "+" : "") << (float)(score / 100.0);  // Add + signe for white advantage

	}
	computerScore->setText(qs);
}