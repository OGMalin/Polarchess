#include "MainWindow.h"
#include "AboutDialog.h"
#include "ImportPgnDialog.h"
#include "DatabaseDialog.h"
#include "../Common/Pgn.h"
#include "../Common/ChessGame.h"
#include <QMenuBar>
#include <QSettings>
#include <QApplication>
#include <QRect>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>
#include <QSplitter>
#include <QPushButton>
#include <QStatusBar>
#include <QClipboard>
#include <QMimeData>
#include <QTextStream>
#include <string>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	dataPath = QStandardPaths::locate(QStandardPaths::DocumentsLocation, QCoreApplication::organizationName(), QStandardPaths::LocateDirectory);
	dataPath += "/" + QCoreApplication::applicationName();
	write = -1;
	inTraining = false;

	createMenu();
	createStatusbar();

	statusBar();
	loadLanguage();

	hSplitter = new QSplitter(Qt::Horizontal);
	v1Splitter = new QSplitter(Qt::Vertical);
	v2Splitter = new QSplitter(Qt::Vertical);

	boardwindow = new BoardWindow;
	openingwindow = new OpeningWindow;
	commentwindow = new CommentWindow;
	pathwindow = new PathWindow;
	enginewindow = new EngineWindow;
	movewindow = new MoveTableWindow;

	v1Splitter->addWidget(openingwindow);
	v1Splitter->addWidget(boardwindow);
	v1Splitter->addWidget(enginewindow);
	v2Splitter->addWidget(pathwindow);
	v2Splitter->addWidget(movewindow);
	v2Splitter->addWidget(commentwindow);
	hSplitter->addWidget(v1Splitter);
	hSplitter->addWidget(v2Splitter);

	setCentralWidget(hSplitter);

	readSettings();

	retranslateUi();

	Base[THEORY] = new Database("theory");
	Base[REPWHITE] = new Database("white");
	Base[REPBLACK] = new Database("black");
	statistics = new Statistics();
	computer = new Computer();
	currentPath = new Path();
	training = new Training();
	training->SetDatabase(WHITE, Base[REPWHITE]);
	training->SetDatabase(BLACK, Base[REPBLACK]);
	movewindow->computer = computer;

	connect(boardwindow, SIGNAL(moveEntered(ChessMove&)), this, SLOT(moveEntered(ChessMove&)));
	connect(movewindow, SIGNAL(moveSelected(ChessMove&)), this, SLOT(moveSelected(ChessMove&)));
	connect(movewindow, SIGNAL(moveDelete(int, const ChessMove&)), this, SLOT(moveDelete(int, const ChessMove&)));
	connect(movewindow, SIGNAL(moveSetAsMain(int, const ChessMove&)), this, SLOT(moveSetAsMain(int, const ChessMove&)));
	connect(movewindow, SIGNAL(needRefresh()), this, SLOT(childNeedRefresh()));
	connect(movewindow, SIGNAL(addMoveComment(int, int, QString&)), this, SLOT(addMoveComment(int, int, QString&)));
	connect(pathwindow, SIGNAL(pathToDB(int)), this, SLOT(pathToDB(int)));
	connect(pathwindow, SIGNAL(pathSelected(int)), this, SLOT(pathSelected(int)));
	connect(pathwindow, SIGNAL(pathCopy()), this, SLOT(pathCopy()));
	connect(pathwindow, SIGNAL(pathPaste()), this, SLOT(pathPaste()));
	connect(commentwindow, SIGNAL(commentChanged(QString&)), this, SLOT(commentChanged(QString&)));
	connect(enginewindow, SIGNAL(enginePV(ComputerDBEngine&, ChessBoard&)), this, SLOT(enginePV(ComputerDBEngine&, ChessBoard&)));

	// Open default databases
	if (!dataTheory.isEmpty())
		if (!Base[THEORY]->open(dataTheory))
			Base[THEORY]->create(dataTheory,THEORY);
	if (!dataWhite.isEmpty())
		if (!Base[REPWHITE]->open(dataWhite))
			Base[REPWHITE]->create(dataWhite, REPWHITE);
	if (!dataBlack.isEmpty())
		if (!Base[REPBLACK]->open(dataBlack))
			Base[REPBLACK]->create(dataBlack, REPBLACK);
	if (!dataStatistics.isEmpty())
		if (!statistics->open(dataStatistics))
			statistics->create(dataStatistics);
	if (!dataComputer.isEmpty())
		if (!computer->open(dataComputer))
			computer->create(dataComputer);
	if (!dataTraining.isEmpty())
		if (!training->open(dataTraining))
			training->create(dataTraining);

	ChessBoard board = currentPath->getPosition();

	readDB();

	enginewindow->setPosition(board);
	boardwindow->setPosition(board);
	enginewindow->setPosition(board, currentPath->current() / 2 + 1);

	updateMenu();
	updateWindow();
}

MainWindow::~MainWindow()
{
	delete currentPath;
	delete training;
}

void MainWindow::createMenu()
{
	// File menu
	fileMenu = menuBar()->addMenu("*");
	fileImportMenu = fileMenu->addMenu("Import");
	importPgnAct = fileImportMenu->addAction("Import pgnfiles", this, &MainWindow::fileImportPgn);
	fileMenu->addSeparator();
	exitAct = fileMenu->addAction("*", this, &QWidget::close);

	bookMenu = menuBar()->addMenu("Book");
	bookWriteMenu = bookMenu->addMenu("Write enable");
	writeAct[THEORY] = bookWriteMenu->addAction("Write to theory book", this, &MainWindow::bookWriteTheory);
	writeAct[REPWHITE] = bookWriteMenu->addAction("Write to White repertoire book", this, &MainWindow::bookWriteWhite);
	writeAct[REPBLACK] = bookWriteMenu->addAction("Write to Black repertoire book", this, &MainWindow::bookWriteBlack);
	bookMenu->addSeparator();

	trainingMenu = menuBar()->addMenu("Training");
	trainingStartMenu = trainingMenu->addMenu("Start training");
	startTrainingBothAct = trainingStartMenu->addAction("Start training", this, &MainWindow::trainingStartBoth);
	startTrainingWhiteAct = trainingStartMenu->addAction("Start training for White", this, &MainWindow::trainingStartWhite);
	startTrainingBlackAct = trainingStartMenu->addAction("Start training for Black", this, &MainWindow::trainingStartBlack);
	startTrainingPosBothAct = trainingStartMenu->addAction("Start training from current position", this, &MainWindow::trainingStartPosBoth);
	startTrainingPosWhiteAct = trainingStartMenu->addAction("Start training from current position for White", this, &MainWindow::trainingStartPosWhite);
	startTrainingPosBlackAct = trainingStartMenu->addAction("Start training from current position for Black", this, &MainWindow::trainingStartPosBlack);
	stopTrainingAct = trainingMenu->addAction("Stop training", this, &MainWindow::trainingStop);
	bookMenu->addSeparator();
	clearTrainingAct = trainingMenu->addAction("Clear trainingdata", this, &MainWindow::trainingClearData);
	createTrainingAct = trainingMenu->addAction("Create training", this, &MainWindow::trainingCreate);

	settingsMenu = menuBar()->addMenu("*");
	setupDatabaseAct = settingsMenu->addAction("*", this, &MainWindow::setupDatabase);
	//	fileMenu->addSeparator();
	langMenu = settingsMenu->addMenu("*");
	engAct = langMenu->addAction(QIcon(":/icon/GB.png"), "*");
	engAct->setCheckable(true);
	engAct->setData("gb");
	norAct = langMenu->addAction(QIcon(":/icon/NO.png"), "*");
	norAct->setCheckable(true);
	norAct->setData("nb");
	langGroup = new QActionGroup(this);
	connect(langGroup, SIGNAL(triggered(QAction *)), this, SLOT(slotLanguageChanged(QAction *)));
	langGroup->addAction(engAct);
	langGroup->addAction(norAct);
	if (locale == "nb")
		norAct->setChecked(true);
	else
		engAct->setChecked(true);

	// Setting up the toolbar
	toolbar = addToolBar("Toolbar");
	toolbar->setObjectName("MainToolbar");
	for (int i = 0; i < 3; i++)
	{
		toolbar->addAction(writeAct[i]);
		writeAct[i]->setCheckable(true);
		writeAct[i]->setDisabled(true);
	}

	// No database opened as default
	importPgnAct->setDisabled(true);

}

void MainWindow::retranslateUi()
{
	fileMenu->setTitle(tr("File"));
	exitAct->setText(tr("Exit"));

	//boardMenu->setTitle(tr("Board"));
	//flipAct->setText(tr("Flip board"));

	//gameMenu->setTitle(tr("Game"));
	//newGameAct->setText(tr("New game"));
	//abortAct->setText(tr("Abort"));
	//resignAct->setText(tr("Resign"));

	settingsMenu->setTitle(tr("Settings"));
	setupDatabaseAct->setText(tr("setup database"));
	langMenu->setTitle(tr("Language"));
	if (locale == "nb")
		langMenu->setIcon(QIcon(":/icon/NO.png"));
	else
		langMenu->setIcon(QIcon(":/icon/GB.png"));
	engAct->setText(tr("English"));
	norAct->setText(tr("Norwegian"));
	//defAct->setText(tr("Set default settings"));
	//helpMenu->setTitle(tr("Help"));
	//aboutAct->setText(tr("About..."));
}

void MainWindow::updateMenu()
{
	switch (write)
	{
		case 0:
			writeAct[0]->setChecked(true);
			writeAct[1]->setChecked(false);
			writeAct[2]->setChecked(false);
			break;
		case 1:
			writeAct[0]->setChecked(false);
			writeAct[1]->setChecked(true);
			writeAct[2]->setChecked(false);
			break;
		case 2:
			writeAct[0]->setChecked(false);
			writeAct[1]->setChecked(false);
			writeAct[2]->setChecked(true);
			break;
		default:
			writeAct[0]->setChecked(false);
			writeAct[1]->setChecked(false);
			writeAct[2]->setChecked(false);
			break;
	}
	commentwindow->write = write;
	int open = 0;
	for (int i = 0; i < 3; i++)
	{
		if (Base[i]->isOpen())
		{
			writeAct[i]->setDisabled(false);
			++open;
		}else
		{
			writeAct[i]->setDisabled(true);
		}
	}
	if (open==0)
		importPgnAct->setDisabled(true);
	else
		importPgnAct->setDisabled(false);
}

void MainWindow::updateWindow()
{
	movewindow->refresh(bde[THEORY], bde[REPWHITE], bde[REPBLACK], sde, cde, currentPath->getPosition(), currentPath->current() / 2 + 1);
	commentwindow->refresh(bde[THEORY].comment, bde[REPWHITE].comment, bde[REPBLACK].comment);
	pathwindow->refresh(currentPath);

	if(inTraining)
	{
		movewindow->setVisible(false);
		commentwindow->setVisible(false);
		enginewindow->setVisible(false);
	}
	else
	{
		movewindow->setVisible(true);
		commentwindow->setVisible(true);
		enginewindow->setVisible(true);
	}
}

void MainWindow::readDB()
{
	// Change to read from both db
	ChessBoard cb = currentPath->getPosition();

	for (int i = 0; i < 3; i++)
	{
		bde[i] = Base[i]->find(cb);
	}
	sde = statistics->find(cb);
	cde = computer->find(cb);

}

void MainWindow::createStatusbar()
{
	statusWatch = new StatusWatch();
	statusBar()->addPermanentWidget(statusWatch);
}

void MainWindow::writeSettings()
{
	QSettings settings;
	settings.setValue("mainWindowGeometry", saveGeometry());
	settings.setValue("mainWindowState", saveState());
	settings.setValue("v1State", v1Splitter->saveState());
	settings.setValue("v2State", v2Splitter->saveState());
	settings.setValue("hState", hSplitter->saveState());
	settings.setValue("dataTheory", dataTheory);
	settings.setValue("dataWhite", dataWhite);
	settings.setValue("dataBlack", dataBlack);
	settings.setValue("dataStatistics", dataStatistics);
	settings.setValue("dataComputer", dataComputer);
	settings.setValue("dataTraining", dataTraining);
	settings.setValue("language", locale);
}

void MainWindow::readSettings()
{
	QSettings settings;
	restoreGeometry(settings.value("mainWindowGeometry").toByteArray());
	restoreState(settings.value("mainWindowState").toByteArray());
	v1Splitter->restoreState(settings.value("v1State").toByteArray());
	v2Splitter->restoreState(settings.value("v2State").toByteArray());
	hSplitter->restoreState(settings.value("hState").toByteArray());
	dataPath = settings.value("dataPath",dataPath).toString();
	dataTheory = settings.value("dataTheory", dataPath + "/Theory.pbk").toString();
	dataWhite = settings.value("dataWhite", dataPath + "/White.pbk").toString();
	dataBlack = settings.value("dataBlack", dataPath + "/Black.pbk").toString();
	dataStatistics = settings.value("dataStatistics", dataPath + "/Statistics.pst").toString();
	dataComputer = settings.value("dataComputer", dataPath + "/Computer.pcp").toString();
	dataTraining = settings.value("dataTraining", dataPath + "/Training.ptr").toString();
	locale = settings.value("language", QString()).toString();
	if (locale.isEmpty())
	{
		// Find the systems default language
		locale = QLocale::system().name();
		locale.truncate(locale.lastIndexOf('_'));
	}

	/*
	QSettings settings;
	QByteArray maingeometry = settings.value("maingeometry", QByteArray()).toByteArray();
	if (maingeometry.isEmpty())
	{
		const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
		resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
		move((availableGeometry.width() - width()) / 2,
			(availableGeometry.height() - height()) / 2);
	}
	else
	{
		restoreGeometry(maingeometry);
	}
	QString version = settings.value("Version", QString()).toString();
	if (version.isEmpty())
		settings.setValue("Version", QCoreApplication::applicationVersion());
		*/
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	writeSettings();
//	event->accept();
}

//void MainWindow::fileOpen(int type)
//{
//	QMessageBox msgbox;
//	QString path = QFileDialog::getOpenFileName(this, "Open book", dataPath, "Book files (*.pbk)");
//	if (!path.isEmpty())
//	{
//		if (!Base[type]->open(path))
//		{
//			msgbox.setText("Can't open book");
//			msgbox.exec();
//			return;
//		}
//		bde[type] = Base[type]->find(currentPath->getPosition());
//
//		updateWindow();
//
//		if (write == type)
//		{
//			write = -1;
//		}
//		updateMenu();
//	}
//}
//
//void MainWindow::fileNew(int type)
//{
//	QString path = QFileDialog::getSaveFileName(this, "Open book", dataPath, "Book files (*.pbk)");
//	if (!path.isEmpty())
//	{
//		QFile file(path);
//		if (file.exists())
//		{
//			QMessageBox msgbox;
//			msgbox.setText("The book allready exist. Do you want to delete it?");
//			msgbox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
//			if (msgbox.exec() != QMessageBox::Ok)
//				return;
//			file.remove();
//		}
//		Base[type]->create(path, type);
//		bde[type].clear();
//		bde[type].board = currentPath->getStartPosition();
//		Base[type]->add(bde[type]);
//
//		bde[type] = Base[type]->find(currentPath->getPosition());
//		
//		updateWindow();
//
//		if (write == type)
//			write = -1;
//		updateMenu();
//	}
//}
//
//void MainWindow::fileClose(int type)
//{
//	Base[type]->close();
//	bde[type].clear();
//
//	if (write == type)
//		write = -1;
//
//	updateMenu();
//	updateWindow();
//}


void MainWindow::bookWrite(int type)
{
	if (write == type)
		write = -1;
	else
		write = type;
	updateMenu();
}

void MainWindow::flipBoard()
{
	boardwindow->flip();
	boardwindow->update();
}

void MainWindow::aboutDialog()
{
	AboutDialog dialog(this);
	dialog.exec();
}

void MainWindow::moveSelected(ChessMove& move)
{
	BookDBMove bm;
	ChessBoard board;

	// Do the move if it is legal
	if (!currentPath->add(move))
		return;


	if (write >= 0)
	{
		// Save the move if it doesn't exist
		if (!bde[write].moveExist(move))
		{
			bm.clear();
			bm.move = move;
			bde[write].movelist.append(bm);
			Base[write]->add(bde[write]);
		}
	}

	// Change to read from both db
	board = currentPath->getPosition();

	readDB();

	boardwindow->setPosition(board);
	enginewindow->setPosition(board, currentPath->current() / 2 + 1);

	updateWindow();
}

void MainWindow::moveDelete(int rep, const ChessMove& move)
{
	if ((rep < 0) || (rep > 2))
		return;
	bde[rep].deleteMove(move);
	Base[rep]->add(bde[rep]);
	updateWindow();
}

void MainWindow::moveSetAsMain(int rep, const ChessMove& move)
{
	if ((rep < 1) || (rep > 2))
		return;
	bde[rep].setFirst(move);
	Base[rep]->add(bde[rep]);
	updateWindow();
}

void MainWindow::addMoveComment(int rep, int movenr, QString& comment)
{
	//if ((rep < 0) || (rep > 2))
	//	return;
	//if (bde[rep].movelist.size() <= movenr)
	//	return;
	//bde[rep].movelist[movenr].comment = comment;
	//Base[rep]->add(bde[rep]);
	//updateWindow();
}

void MainWindow::moveEntered(ChessMove& move)
{
	ChessBoard board = currentPath->getPosition();
	BookDBMove bm;
	int i;

	if (!board.isLegal(move))
	{
		boardwindow->setPosition(board);
		return;
	}
		
	if (inTraining)
	{
		int score=0;
		if (trainingLine.isCorrect(move))
		{
			statusBar()->showMessage(QString("correct move"), 5000);
			currentPath->add(move);
			boardwindow->setPosition(currentPath->getPosition());
			ChessMove nextMove;
			if (trainingLine.nextMove(nextMove))
			{
				currentPath->add(nextMove);
				board = currentPath->getPosition();

				boardwindow->setPosition(board);
				enginewindow->setPosition(board, currentPath->current() / 2 + 1);

				updateWindow();
				return;
			}
			else
			{
				currentPath->current(currentPath->current() - 1);
				if (!trainingStat.moveerror)
					score = 1;
				else
					score = -1;
				training->updateScore(trainingLine.color, currentPath->getPosition(), trainingLine.rowid, trainingLine.score + score);
				statusBar()->showMessage(QString("Next line"), 5000);
				trainingStart(trainingColor,trainingBoard);
				return;
			}
		}
		else
		{
			boardwindow->setPosition(currentPath->getPosition());
			++trainingStat.moveerror;
			statusBar()->showMessage(QString("Wrong move"), 5000);

			ChessMove m=trainingLine.currentMove();
			boardwindow->markSquare(SQUARE64(m.fromSquare));
			updateWindow();
			return;
		}

	}

	// Do the move if it is legal
	if (!currentPath->add(move))
	{
		boardwindow->setPosition(board);
		return;
	}


	if (write >= 0)
	{
		// Save the move if it doesn't exist
		if (!bde[write].moveExist(move))
		{
			bm.clear();
			bm.move = move;
			bde[write].movelist.append(bm);
			Base[write]->add(bde[write]);
		}
	}

	// Change to read from both db
	board = currentPath->getPosition();

	readDB();

	boardwindow->setPosition(board);
	enginewindow->setPosition(board,currentPath->current()/2+1);

	updateWindow();
}

void MainWindow::pathSelected(int ply)
{
	if (inTraining)
		return;

	currentPath->current(ply);

	ChessBoard board = currentPath->getPosition();

	readDB();

	boardwindow->setPosition(board);
	enginewindow->setPosition(board, currentPath->current() / 2 + 1);
	updateWindow();
}

void MainWindow::pathToDB(int rep)
{
	if (inTraining)
		return;
	if (Base[rep]->isOpen())
	{
		BookDBEntry bd;
		PathEntry pe;
		BookDBMove bm;
		for (int i = 0; i < currentPath->current(); i++)
		{
			pe = currentPath->getEntry(i);
			bd = Base[rep]->find(pe.board);
			if (!bd.moveExist(pe.move))
			{
				bm.clear();
				bm.move = pe.move;
				bd.movelist.append(bm);
				Base[rep]->add(bd);
			}

		}
	}
}

void MainWindow::commentChanged(QString& comment)
{
	if (write >= 0)
	{
		bde[write].comment = comment;
		Base[write]->add(bde[write]);
	}
}

void MainWindow::fileImportPgn()
{
	int rep;
	int moves=999;
	ImportPgnDialog dialog(this);
	dialog.setItems(Base[THEORY]->isOpen(), Base[REPWHITE]->isOpen(), Base[REPBLACK]->isOpen(), moves, true);
	if (dialog.exec() == QDialog::Rejected)
		return;
	bool theory, white, black, variation, comment;
	QString path;
	dialog.getItems(path, theory, white, black, moves, comment, variation);

	if (theory)
		rep = THEORY;
	else if (white)
		rep = REPWHITE;
	else if (black)
		rep = REPBLACK;
	else
		return;

	dialog.importPgnFile(this, Base[rep], path, moves, comment, variation);

	// Change to read from both db
	ChessBoard board = currentPath->getPosition();

	readDB();

	boardwindow->setPosition(board);

	updateWindow();
}

void MainWindow::trainingClearData()
{
	Base[REPWHITE]->clearAllTrainingData();
	Base[REPBLACK]->clearAllTrainingData();
}

void MainWindow::trainingCreate()
{
	training->createLines(this);
}

void MainWindow::trainingStart(int color, ChessBoard& board)
{
	int i;
	trainingStat.clear();
	trainingColor = color;
	trainingBoard = board;
	training->get(trainingLine, color, trainingBoard);
	if (trainingLine.moves.size() == 0)
		return;
	boardwindow->flip(trainingLine.color == BLACK);
	ChessBoard cb;
	cb.setStartposition();
	currentPath->clear();
	trainingLine.current = 0;
	// Go to startposition of this training line
	for (i = 0; i < trainingLine.moves.size(); i++)
	{
		if (cb == trainingBoard)
			break;
		currentPath->add(trainingLine.moves[i].move);
		cb.doMove(trainingLine.moves[i].move,false);
		++trainingLine.current;
	}

	// Startposition not found
	if (i == trainingLine.moves.size())
		return;
	
	// If wrong color to move go one move forward
	if (currentPath->getPosition().toMove != trainingLine.color)
	{
		currentPath->add(trainingLine.moves[i].move);
		cb.doMove(trainingLine.moves[i].move, false);
		++trainingLine.current;
	}
	inTraining = true;
	write = -1;
	boardwindow->setPosition(cb);
	updateWindow();
}

void MainWindow::trainingStartBoth()
{
	ChessBoard cb;
	cb.setStartposition();
	trainingStart(-1, cb);
}

void MainWindow::trainingStartWhite()
{
	ChessBoard cb;
	cb.setStartposition();
	trainingStart(WHITE, cb);
}

void MainWindow::trainingStartBlack()
{
	ChessBoard cb;
	cb.setStartposition();
	trainingStart(BLACK, cb);
}

void MainWindow::trainingStartPosBoth()
{
	trainingStart(-1, currentPath->getPosition());
}

void MainWindow::trainingStartPosWhite()
{
	trainingStart(WHITE, currentPath->getPosition());
}

void MainWindow::trainingStartPosBlack()
{
	trainingStart(BLACK, currentPath->getPosition());
}


void MainWindow::trainingStop()
{
	inTraining = false;
	updateWindow();
}

void MainWindow::enginePV(ComputerDBEngine& ce, ChessBoard& cb)
{
	// cb could either be current position or a freezing position.
	computer->add(ce, cb);
}

void MainWindow::setLanguage()
{
	this->repaint();
}


void MainWindow::slotLanguageChanged(QAction* action)
{
	if (0 != action) {
		// load the language dependant on the action content
		locale = action->data().toString();
		loadLanguage();
		updateWindow();
	}
}

void MainWindow::loadLanguage()
{
	if (locale == "nb")
	{
		if (translator.isEmpty())
		{
			if (translator.load(":/language/book_nb.qm"))
				qApp->installTranslator(&translator);
		}
		else
		{
			qApp->installTranslator(&translator);
		}
		return;
	}

	if (!translator.isEmpty())
		qApp->removeTranslator(&translator);
	return;
}

void MainWindow::changeEvent(QEvent* event)
{
	if (0 != event) {
		switch (event->type()) {
			// this event is send if a translator is loaded
		case QEvent::LanguageChange:
			retranslateUi();
			break;

			// this event is send, if the system, language changes
		case QEvent::LocaleChange:
		{
			locale = QLocale::system().name();
			locale.truncate(locale.lastIndexOf('_'));
			loadLanguage();
		}
		break;
		}
	}
	QMainWindow::changeEvent(event);
}

void MainWindow::pathCopy()
{
	QStringList qsl;
	QString qs;
	QClipboard* clipboard = QApplication::clipboard();
	currentPath->getMoveList(qsl,NULL,SAN);
	QTextStream(&qs) << "[Event \"?\"]" << endl << "[Site \"?\"]" << endl << "[Date \"????.??.??\"]" << endl << "[Round \"?\"]" << endl << "[White \"?\"]" << endl << "[Black \"?\"]" << endl << "[Result \"*\"]" << endl << endl;
	int i;
	for (i = 0; i < qsl.size(); i++)
	{
		if (i % 2)
			QTextStream(&qs) << " ";
		else
			QTextStream(&qs) << " " << (2+i)/2 << ". ";
		QTextStream(&qs) << qsl[i];
	}
	QTextStream(&qs) << " *" << endl << endl;

	clipboard->setText(qs);
}

void MainWindow::pathPaste()
{
	QString qs;
	Pgn pgn;
	ChessGame game;
	ChessMove move;
	std::string ss;
	const QClipboard *clipboard = QApplication::clipboard();
	const QMimeData *mimeData = clipboard->mimeData();
	if (!mimeData->hasText())
		return;
	qs = clipboard->text();
	// Is pgn format
	if (qs.indexOf("[Event ", 0) >= 0)
	{
		if (pgn.read(game, 1, 999, false, qs.toLatin1()))
		{
			currentPath->clear();
			game.toStart();
			while (game.getMove(move, ss, 0))
			{
				currentPath->add(move);
				game.doMove(0);
			}
			readDB();

			ChessBoard board = currentPath->getPosition();
			boardwindow->setPosition(board);
			enginewindow->setPosition(board, currentPath->current() / 2 + 1);
			updateWindow();
		}
	}
}

void MainWindow::childNeedRefresh()
{
	readDB();
	updateWindow();
}

void MainWindow::setupDatabase()
{
	DatabaseDialog dialog(this, Base[THEORY], Base[REPWHITE], Base[REPBLACK], statistics);
	dialog.setItems(dataTheory, dataWhite, dataBlack, dataTraining, dataComputer, dataStatistics);
	if (dialog.exec() == QDialog::Rejected)
		return;
	dialog.getItems(dataTheory, dataWhite, dataBlack, dataTraining, dataComputer, dataStatistics);

	if (!dataStatistics.isEmpty())
	{
		if (!statistics->open(dataStatistics))
			statistics->create(dataStatistics);
	}
	else
	{
		statistics->close();
	}
	if (!dataComputer.isEmpty())
	{
		if (!computer->open(dataComputer))
			computer->create(dataComputer);
	}
	else
	{
		computer->close();
	}
	if (!dataTraining.isEmpty())
	{
		if (!training->open(dataTraining))
			training->create(dataTraining);
	}
	else
	{
		training->close();
	}

	write = -1;
	readDB();
	updateWindow();
	updateMenu();
}