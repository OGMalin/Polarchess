#include "MainWindow.h"
#include "AboutDialog.h"
#include "NewGameDialog.h"
//#include "ClockWindow.h"
//#include "EngineWindow.h"
//#include "Database.h"
//#include "Scoresheet.h"
//#include "Engine.h"
//#include "../Common/BoardWindow.h"
//#include "../Common/QChessGame.h"
//#include <QIcon>
//#include <QSplitter>
#include <QMenuBar>
//#include <QToolBar>
//#include <QEvent>
#include <QSettings>
#include <QCoreApplication>
#include <QLocale>
#include <QCloseEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QStatusBar>
#include <QRandomGenerator>
#include <QDate>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextToSpeech>
#include <QLocale>
#include <QVoice>
#include <QClipboard>
#include <QTextStream>

//#include <QEventLoop>

MainWindow::MainWindow()
{
	currentGame = new QChessGame();
	currentGame->newGame();
	running = false;
	dgt = NULL;
	autoSaveToPgn = true;

	statusBar();
	loadLanguage();


	hSplitter = new QSplitter(Qt::Horizontal);
	vSplitter = new QSplitter(Qt::Vertical);

	boardwindow = new BoardWindow(this);
	scoresheet = new Scoresheet(this);
	clockwindow = new ClockWindow(this);
	enginewindow = new EngineWindow(this);
	sound = new Sound(this);
	book = new Book();

	hSplitter->addWidget(boardwindow);
	hSplitter->addWidget(vSplitter);
	vSplitter->addWidget(clockwindow);
	vSplitter->addWidget(scoresheet);
//	vSplitter->addWidget(enginewindow);

	setCentralWidget(hSplitter);

	createMenu();
	retranslateUi();

	engine = new Engine();
	enginestate = 0;
//	QString name = "Engine.exe";
//	QString dir = QCoreApplication::applicationDirPath();
//	playEngine->setEngine(name, dir);

	readSettings();

	saveToPgnAct->setChecked(autoSaveToPgn);

	database = new Database();
//	connect(engine, SIGNAL(engineMessage(const QString&)), this, SLOT(playEngineMessage(const QString&)));
	connect(engine, SIGNAL(engineStarted()), this, SLOT(playEngineStarted()));
	connect(engine, SIGNAL(engineMove(const QString&, const QString&)), this, SLOT(playEngineMove(const QString&, const QString&)));
	connect(clockwindow, SIGNAL(clockAlarm(int)),this, SLOT(clockAlarm(int)));
	connect(boardwindow, SIGNAL(moveEntered(ChessMove&)), this, SLOT(moveEntered(ChessMove&)));
	connect(scoresheet, SIGNAL(moveSelected(int)), this, SLOT(moveSelected(int)));
	// Check and update engine options
	if (!installedEngine.isEmpty())
	{
		enginestate = 2;
		if (!engine->load(installedEngine))
			enginestate = 0;
	}
}

MainWindow::~MainWindow()
{
	/*
	delete boardwindow;
	delete scoresheet;
	delete langGroup;
*/
	delete currentGame;
	delete engine;
	delete book;
}

// The text in the menu are set in retranslateUi to be able to switch language 'on the fly'.
void MainWindow::createMenu()
{
	//Main menues

	// File menu
	fileMenu = menuBar()->addMenu("*");
	exitAct = fileMenu->addAction("*", this, &QWidget::close);

	//Board menu
	boardMenu = menuBar()->addMenu("*");
	flipAct = boardMenu->addAction("*", this, &MainWindow::flipBoard);

	// Game menu
	gameMenu = menuBar()->addMenu("*");
	newGameAct = gameMenu->addAction(QIcon(":/icon/board48.png"),"*",this,&MainWindow::newGame);
	abortAct = gameMenu->addAction(QIcon(":/icon/abort48.png"), "*", this, &MainWindow::abort);
	resignAct = gameMenu->addAction(QIcon(":/icon/resign48.png"), "*", this, &MainWindow::resign);
	copyAct = gameMenu->addAction("*", this, &MainWindow::copy,QKeySequence::Copy);

	// Settings menu
	settingsMenu = menuBar()->addMenu("*");
	soundAct = settingsMenu->addAction("*", this, &MainWindow::setSound);
	installEngineAct = settingsMenu->addAction("*", this, &MainWindow::installEngine);
	saveToPgnAct = settingsMenu->addAction("*", this, &MainWindow::saveToPgn);
	saveToPgnAct->setCheckable(true);
	useDgtAct = settingsMenu->addAction("*", this, &MainWindow::useDgt);
	useDgtAct->setCheckable(true);
	settingsMenu->addSeparator();
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
	defAct = settingsMenu->addAction("*", this, &MainWindow::setDefaultSettings);

	// Help menu
	helpMenu = menuBar()->addMenu("*");
	aboutAct = helpMenu->addAction("*", this, &MainWindow::aboutDialog);

	// Setting up the toolbar
	toolbar = addToolBar("Toolbar");
	toolbar->addAction(newGameAct);
	toolbar->addSeparator();
	toolbar->addSeparator();
	toolbar->addAction(abortAct);
	toolbar->addAction(resignAct);
}

void MainWindow::retranslateUi()
{
	fileMenu->setTitle(tr("File"));
	exitAct->setText(tr("Exit"));

	boardMenu->setTitle(tr("Board"));
	flipAct->setText(tr("Flip board"));

	gameMenu->setTitle(tr("Game"));
	newGameAct->setText(tr("New game"));
	abortAct->setText(tr("Abort"));
	resignAct->setText(tr("Resign"));
	copyAct->setText(tr("Copy"));

	settingsMenu->setTitle(tr("Settings"));
	soundAct->setText(tr("Sound"));
	installEngineAct->setText(tr("Install Engine"));
	saveToPgnAct->setText(tr("Save to pgn"));
	useDgtAct->setText(tr("Use DGT Electronic board"));
	langMenu->setTitle(tr("Language"));
	if (locale == "nb")
		langMenu->setIcon(QIcon(":/icon/NO.png"));
	else
		langMenu->setIcon(QIcon(":/icon/GB.png"));
	engAct->setText(tr("English"));
	norAct->setText(tr("Norwegian"));
	defAct->setText(tr("Set default settings"));
	helpMenu->setTitle(tr("Help"));
	aboutAct->setText(tr("About..."));
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
	}
}

void MainWindow::loadLanguage()
{
	if (locale == "nb")
	{
		if (translator.isEmpty())
		{
			if (translator.load(":/language/gui_nb.qm"))
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

void MainWindow::closeEvent(QCloseEvent* event)
{
	writeSettings();
	event->accept();
}

void MainWindow::writeSettings()
{
	player.save();
	if (dgt)
		dgtSetting = dgt->setting();
	QSettings settings;
	settings.setValue("maingeometry", saveGeometry());
//	settings.setValue("hgeometry", hSplitter->saveState());
//	settings.setValue("vgeometry", vSplitter->saveState());
	settings.setValue("language", locale);
	settings.setValue("autosave", autoSaveToPgn);
	settings.setValue("player", gameSetting.player);
	settings.beginGroup("lastgame");
	settings.setValue("skill", gameSetting.engineskill);
	settings.setValue("autoskill", gameSetting.autoskill);
	settings.setValue("personality", gameSetting.personality);
	settings.setValue("elo", gameSetting.engineelo);
	settings.setValue("fullstrength", gameSetting.fullstrength);
	settings.setValue("time", gameSetting.startTime);
	settings.setValue("increment", gameSetting.startTimeInc);
	settings.setValue("suddendeath", gameSetting.suddenDeathTime);
	settings.setValue("rated", gameSetting.rated);
	settings.setValue("color", gameSetting.color);
	settings.setValue("book", gameSetting.book);
	settings.setValue("defaultbook", gameSetting.defaultBook);
	settings.endGroup();

	settings.beginGroup("sound");
	settings.setValue("language", sound->setting.language);
	settings.setValue("usage", sound->setting.usage);
	settings.endGroup();

	settings.beginGroup("dgt");
	settings.setValue("port", dgtSetting.port);
	settings.setValue("autorotate", dgtSetting.autoRotate);
	settings.setValue("delay", dgtSetting.delay);
	settings.endGroup();

	settings.setValue("engine", installedEngine);
}

void MainWindow::readSettings()
{
	QSettings settings;
	QByteArray maingeometry = settings.value("maingeometry", QByteArray()).toByteArray();
//	QByteArray hgeometry = settings.value("hgeometry", QByteArray()).toByteArray();
//	QByteArray vgeometry = settings.value("vgeometry", QByteArray()).toByteArray();
	locale = settings.value("language", QString()).toString();
	autoSaveToPgn = settings.value("autosave", "true").toBool();
	gameSetting.player = settings.value("player", QString()).toString();
	// Last game setup
	settings.beginGroup("lastgame");
	gameSetting.engineskill = settings.value("skill", "0").toInt();
	gameSetting.autoskill = settings.value("autoskill", "false").toBool();
	gameSetting.personality = settings.value("personality", "").toString();
	gameSetting.engineelo = settings.value("elo", "0").toInt();
	gameSetting.fullstrength = settings.value("fullstrength", "false").toBool();
	gameSetting.startTime = settings.value("time", "900").toInt();
	gameSetting.startTimeInc = settings.value("increment", "10").toInt();
	gameSetting.suddenDeathTime = settings.value("suddendeath", "0").toInt();
	gameSetting.rated = settings.value("rated", "false").toBool();
	gameSetting.color = settings.value("color", "0").toInt();
	gameSetting.book = settings.value("book", "").toString();
	gameSetting.defaultBook = settings.value("defaultbook", "true").toBool();
	settings.endGroup();

	settings.beginGroup("sound");
	sound->setting.language = settings.value("language", "0").toInt();
	sound->setting.usage = settings.value("usage", "0").toInt();
	settings.endGroup();

	settings.beginGroup("dgt");
	dgtSetting.port=settings.value("port", "").toString();
	dgtSetting.autoRotate = settings.value("autorotate", "true").toBool();
	dgtSetting.delay = settings.value("deley", "300").toInt();
	settings.endGroup();

	installedEngine = settings.value("engine", QString()).toString();
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
	/* Problem with read access
	if (!hgeometry.isEmpty())
		hSplitter->restoreState(hgeometry);
	if (!vgeometry.isEmpty())
		vSplitter->restoreState(vgeometry);
	*/
	if (locale.isEmpty())
	{
		// Find the systems default language
		locale = QLocale::system().name();
		locale.truncate(locale.lastIndexOf('_'));
	}
	else
	{
		locale = settings.value("language", QString("gb")).toString();
	}

	player.load(gameSetting.player);
}

void MainWindow::setDefaultSettings()
{
	const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
	resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
	move((availableGeometry.width() - width()) / 2,
		(availableGeometry.height() - height()) / 2);

	// Find the systems default language
	locale = QLocale::system().name();
	locale.truncate(locale.lastIndexOf('_'));
	retranslateUi();
}

void MainWindow::flipBoard()
{
	boardwindow->flip();
	boardwindow->update();
}

void MainWindow::newGame()
{
	if (running)
		return;

	engine->unload();

	if (engine->name.isEmpty())
	{
		QMessageBox msgbox;
		msgbox.setText(tr("You must install an engine before you can play."));
		msgbox.exec();
		return;
	}

	NewGameDialog dialog(this, engine);

	dialog.setDefault(gameSetting);
	if (dialog.exec() == QDialog::Rejected)
		return;
	gameSetting=dialog.getSetting();

	if (gameSetting.book.isEmpty())
		book->close();
	else
		book->open(gameSetting.book);

	currentGame->newGame();
	currentGame->site = "PolarChess GUI";
	currentGame->event = (gameSetting.rated ? "Rated " : "Unrated ");
	switch (gameSetting.gameType)
	{
	case BULLET: currentGame->event += "Bullet "; break;
	case BLITZ: currentGame->event += "Blitz "; break;
	case RAPID: currentGame->event += "Rapid "; break;
	case CLASSICAL: currentGame->event += "Classical "; break;
	}
	currentGame->event += "game";
	currentGame->rated = gameSetting.rated;
	currentGame->gametype = gameSetting.gameType;
	boardwindow->setPosition(currentGame->getPosition().board);
	int color = gameSetting.color;
	if (color == 2)
		color = QRandomGenerator::global()->bounded(0, 2);
	if (color == WHITE)
	{
		currentGame->white=gameSetting.player;
		currentGame->black=gameSetting.engineplayer;
	}
	else
	{
		currentGame->white=gameSetting.engineplayer;
		currentGame->black=gameSetting.player;
	}
	engineColor = OTHERPLAYER(color);
	currentGame->date = QDate().currentDate();
	running = true;

	clockwindow->settime(gameSetting.startTime*1000, gameSetting.startTime*1000);
	clockwindow->start(currentGame->getPosition().board.toMove);
	scoresheet->updateGame(currentGame);
	engine->load(installedEngine);
	if (engineColor == WHITE)
	{
		boardwindow->flip(true);
		doEngineMove();
	}
}

//void MainWindow::playEngineMessage(const QString& msg)
//{
//	statusBar()->showMessage(msg);
//}

void MainWindow::aboutDialog()
{
	AboutDialog dialog(this);
	dialog.exec();
}

void MainWindow::firstTime()
{
	QSettings settings;
	QString version = settings.value("Version",QString()).toString();

	// First time setup
	if (version.isEmpty())
	{
		player.newPlayer(this);
		if (player.name().isEmpty())
		{
			QString p = getenv("USER");
			if (p.isEmpty())
				p = getenv("USERNAME");
			player.name(p);
		}
		gameSetting.player = player.name();

		settings.setValue("Version", QCoreApplication::applicationVersion());
	}

	// Allready installed
	if (version == QCoreApplication::applicationVersion())
		return;


	// Uppgrade
}

void MainWindow::clockAlarm(int color)
{
	if (!canWin(OTHERPLAYER(color)))
	{
		currentGame->result=2;
	}
	else
	{
		currentGame->termination=tr("time forfeit");
		if (color == WHITE)
			currentGame->result = 3;
		else
			currentGame->result = 1;
	}
	endGame();
}

void MainWindow::moveEntered(ChessMove& move)
{
	int tomove = currentGame->toMove();

	// Move not allowed, engine to move.
	if (running && (tomove == engineColor))
	{
		boardwindow->setPosition(currentGame->getPosition().board);
		return;
	}

	// Do the move if it is legal
	if (!currentGame->doMove(move,clockwindow->gettime(tomove)/1000))
	{
		QChessPosition pos = currentGame->getPosition();
		boardwindow->setPosition(currentGame->getPosition().board);
		return;
	}

	if (dgt)
		dgt->setBoard(currentGame->getPosition().board);

	scoresheet->updateGame(currentGame);

	if (gameFinnish())
		return;

	if (!running)
		return;

	// Update clock if needed
	if (gameSetting.startTimeInc)
		clockwindow->addtime(gameSetting.startTimeInc * 1000, tomove);
	if (gameSetting.suddenDeathTime && currentGame->moveCount(tomove) == 40)
		clockwindow->addtime(gameSetting.suddenDeathTime * 1000, tomove);

	// Switch the clock
	clockwindow->start(currentGame->toMove());

	doEngineMove();
}

void MainWindow::endGame()
{
	engine->unload();
	saveGame();
	scoresheet->updateGame(currentGame);
	running = false;
	clockwindow->stop();
}

void MainWindow::playEngineStarted()
{	
	if (enginestate == 1)// Installing
	{
		installedEngine = engine->getPath();
		enginestate = 0;
		QString qs = "Engine: ";
		qs += engine->name;
		qs += "\n";
		qs += "Author: ";
		qs += engine->author;
		statusBar()->showMessage(qs);
		//QMessageBox msgbox;
		//msgbox.setText(qs);
		//msgbox.exec();
		engine->unload();
	}
	else if (enginestate == 2)// Checking
	{
		QString qs = "Engine: ";
		qs += engine->name;
		qs += "\n";
		qs += "Author: ";
		qs += engine->author;
		statusBar()->showMessage(qs);
		enginestate = 0;
		engine->unload();
	}
	else if (enginestate == 3)// playing
	{
		// If engine to move, ask for a move
	}
}

void MainWindow::playEngineMove(const QString& move, const QString& ponder)
{
	if (!running)
		return;
	int player = currentGame->toMove();
	ChessMove m=currentGame->getPosition().board.getMoveFromText(move.toStdString());
	sound->say(currentGame->getPosition().board, m);
	currentGame->doMove(m, clockwindow->gettime(player)/1000);
	boardwindow->setPosition(currentGame->getPosition().board);
	if (dgt)
		dgt->setBoard(currentGame->getPosition().board);
	scoresheet->updateGame(currentGame);
	if (gameFinnish())
		return;
	if (gameSetting.startTimeInc)
		clockwindow->addtime(gameSetting.startTimeInc * 1000, player);
	if (gameSetting.suddenDeathTime && currentGame->moveCount(player) == 40)
		clockwindow->addtime(gameSetting.suddenDeathTime * 1000, player);

	clockwindow->start(currentGame->toMove());
}

void MainWindow::resign()
{
	if (!running)
		return;
	if (engineColor == WHITE)
		currentGame->result = 1;
	else
		currentGame->result = 3;
	endGame();
}

void MainWindow::abort()
{
	if (!running)
		return;
	if (gameSetting.rated)
	{
		if (currentGame->moveCount(OTHERPLAYER(engineColor)) > 1)
		{
			QMessageBox msgbox;
			msgbox.setText(tr("You can't abort a rated game after move 1"));
			msgbox.exec();
			return;
		}
	}
	engine->unload();
	running = false;
	clockwindow->stop();
}

void MainWindow::saveGame()
{
	database->addGame(currentGame, autoSaveToPgn);
}

void MainWindow::installEngine()
{
	QString path = QFileDialog::getOpenFileName(this, tr("Install Engine"), QString(), "UCI Engines (*.exe)");
	if (!path.isEmpty())
	{
		enginestate = 1;
		if (!engine->load(path))
			enginestate = 0;
//		engineFeature.clear();
//		engineFeature.path = path;
//		playEngine->getFeature(engineFeature);
//		playEngine->setEngine(path, QString());
	}
}

void MainWindow::dgtStatus(int status)
{
	if (dgtIcon)
	{
		if (status==0)
			dgtIcon->setIcon(QIcon(":/icon/dgtDisconnect.png"));
		else if (status==1)
			dgtIcon->setIcon(QIcon(":/icon/dgtConnect.png"));
		else if (status == 2)
			dgtIcon->setIcon(QIcon(":/icon/dgtSync.png"));
	}
}

void MainWindow::dgtNewMove(ChessMove* m)
{
	moveEntered(*m);
	boardwindow->setPosition(currentGame->getPosition().board);
}

void MainWindow::dgtNewGame()
{
	newGame();
}

void MainWindow::dgtResult(int result)
{
	QString qs;
	currentGame->result=result;
	endGame();
}

void MainWindow::dgtStatusClicked(bool)
{
	if (dgt->isVisible())
		dgt->hide();
	else
		dgt->show();
}

void MainWindow::useDgt()
{
	if (useDgtAct->isChecked())
	{
		if (!dgt)
		{
			dgt = new DgtBoard(this);
			dgt->setting(dgtSetting);
			connect(dgt, SIGNAL(dgtStatus(int)), this, SLOT(dgtStatus(int)));
			connect(dgt, SIGNAL(dgtNewMove(ChessMove*)), this, SLOT(dgtNewMove(ChessMove*)));
			connect(dgt, SIGNAL(dgtNewGame()), this, SLOT(dgtNewGame()));
			connect(dgt, SIGNAL(dgtResult(int)), this, SLOT(dgtResult(int)));
			dgtIcon = new QToolButton();
			dgtIcon->setIcon(QIcon(":/icon/dgtDisconnect.png"));
			connect(dgtIcon, SIGNAL(clicked(bool)), this, SLOT(dgtStatusClicked(bool)));
			statusBar()->addPermanentWidget(dgtIcon);
			dgt->setBoard(currentGame->getPosition().board);
			if (!dgt->autoConnect())
				dgt->show();
		}
	}
	else
	{
		if (dgt)
		{
			dgtSetting = dgt->setting();
			statusBar()->removeWidget(dgtIcon);
			disconnect(dgtIcon);
			disconnect(dgt);
			delete dgt;
			delete dgtIcon;
			dgt = NULL;
		}
	}
}

void MainWindow::setSound()
{
	sound->showDialog();
}

void MainWindow::saveToPgn()
{
	autoSaveToPgn = autoSaveToPgn ? false : true;
	saveToPgnAct->setChecked(autoSaveToPgn);
}

bool MainWindow::gameFinnish()
{
	// Check for mate/stalemate
	ChessBoard cb = currentGame->getPosition().board;
	if (cb.legalMoves() < 1)
	{
		if (cb.inCheck())
		{
			if (cb.toMove == WHITE)
				currentGame->result = 3;
			else
				currentGame->result = 1;
		}
		else
		{

			currentGame->result = 2;
		}
		endGame();
		return true;
	}

	// 3-fold repetition
	if (currentGame->is3fold())
	{
		currentGame->result = 2;
		return true;
	}

	// 50 moves rule
	if (currentGame->is50move())
	{
		currentGame->result = 2;
		return true;
	}

	// Insufficient material
	int i, p, pt, knights[2], bishops[2];
	knights[WHITE] = knights[BLACK] = bishops[WHITE] = bishops[BLACK] = 0;
	for (i = 0; i < 64; i++)
	{
		p = cb.board[SQUARE128(i)];
		pt = PIECE(p);
		if ((pt == PAWN) || (pt == QUEEN) || (p = ROOK))
			return false;
		switch (pt)
		{
		case KNIGHT:
			++knights[PIECECOLOR(p)];
			break;
		case BISHOP:
			++bishops[PIECECOLOR(p)];
			break;
		}
	}

	// Only kings with only one piece are draw
	if ((knights[WHITE] + knights[BLACK] + bishops[WHITE] + bishops[BLACK]) < 2)
		return true;

	return false;
}

bool MainWindow::canWin(typeColor c)
{
	ChessBoard cb = currentGame->getPosition().board;
	int i, p, pt;

	for (i = 0; i < 64; i++)
	{
		p = cb.board[SQUARE128(i)];
		pt = PIECE(p);
		if (PIECECOLOR(p) == c)
		{
			if ((pt == PAWN) || (pt == KNIGHT) || (pt == BISHOP) || (pt == ROOK) || (pt == QUEEN))
				return true;
		}
	}
	return false;
}

void MainWindow::doEngineMove()
{
	ChessBoard b = currentGame->getPosition().board;

	// First check for book moves
	ChessMove m=book->getMove(b);
	if (!m.empty())
	{
		playEngineMove(b.makeMoveText(m,SAN).c_str(),"");
		return;
	}

	// Let the engine find a move
	int mtg = 0;
	if (gameSetting.suddenDeathTime)
	{
		mtg = 40 - currentGame->moveCount(currentGame->toMove());
		if (mtg < 0)
			mtg = 0;
	}
	b = currentGame->getPosition(0).board;
	MoveList ml = currentGame->movelist();
	engine->search(b, ml, NORMAL_SEARCH, clockwindow->gettime(WHITE), gameSetting.startTimeInc * 1000, clockwindow->gettime(BLACK), gameSetting.startTimeInc * 1000, mtg);
}

void MainWindow::copy()
{
	QString qs;
	QClipboard* clipboard = QApplication::clipboard();
	currentGame->getPgn(qs);
	clipboard->setText(qs);
}

void MainWindow::moveSelected(int ply)
{
	if (running)
		return;
	ChessBoard cb = currentGame->getPosition(ply).board;
	if (dgt)
		dgt->setBoard(cb);
	boardwindow->setPosition(cb);
}