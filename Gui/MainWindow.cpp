#include "MainWindow.h"
#include "AboutDialog.h"
#include "NewGameDialog.h"
#include "ClockWindow.h"
#include "EngineWindow.h"
#include "Database.h"
#include "Scoresheet.h"
#include "../Common/BoardWindow.h"
#include "../Common/Engine.h"
#include "../Common/QChessGame.h"
#include <QIcon>
#include <QSplitter>
#include <QMenuBar>
#include <QToolBar>
#include <QEvent>
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

MainWindow::MainWindow()
{
	currentGame = new QChessGame();
	currentGame->newGame();
	readSettings();
	running = false;
	createMenu();

	statusBar();
	loadLanguage();


	hSplitter = new QSplitter(Qt::Horizontal);
	vSplitter = new QSplitter(Qt::Vertical);

	boardwindow = new BoardWindow;
	scoresheet = new Scoresheet;
	clockwindow = new ClockWindow;
	enginewindow = new EngineWindow;

	hSplitter->addWidget(boardwindow);
	hSplitter->addWidget(vSplitter);
	vSplitter->addWidget(clockwindow);
	vSplitter->addWidget(scoresheet);
//	vSplitter->addWidget(enginewindow);

	setCentralWidget(hSplitter);

	retranslateUi();

	playEngine = new Engine();
	QString name = "Engine.exe";
	QString dir = QCoreApplication::applicationDirPath();
	playEngine->setEngine(name, dir);

	database = new Database();
	connect(playEngine, SIGNAL(engineMessage(const QString&)), this, SLOT(playEngineMessage(const QString&)));
	connect(playEngine, SIGNAL(engineReady()), this, SLOT(playEngineReady()));
	connect(playEngine, SIGNAL(engineMove(const QString&, const QString&)), this, SLOT(playEngineMove(const QString&, const QString&)));
	connect(clockwindow, SIGNAL(clockAlarm(int)),this, SLOT(clockAlarm(int)));
	connect(boardwindow, SIGNAL(moveEntered(ChessMove&)), this, SLOT(moveEntered(ChessMove&)));
}

MainWindow::~MainWindow()
{
	/*
	delete boardwindow;
	delete scoresheet;
	delete langGroup;
*/
	delete currentGame;
	delete playEngine;
}

// The text in the menu are set in retranslateUi to be able to switch language 'on the fly'.
void MainWindow::createMenu()
{
	//Main menues

	// File menu
	fileMenu = menuBar()->addMenu("*");
	exitAct = fileMenu->addAction("*", this, &QWidget::close);

	boardMenu = menuBar()->addMenu("*");
	flipAct = boardMenu->addAction("*", this, &MainWindow::flipBoard);

	gameMenu = menuBar()->addMenu("*");
	newGameAct = gameMenu->addAction(QIcon(":/icon/board48.png"),"*",this,&MainWindow::newGame);
	abortAct = gameMenu->addAction(QIcon(":/icon/abort48.png"), "*", this, &MainWindow::abort);
	resignAct = gameMenu->addAction(QIcon(":/icon/resign48.png"), "*", this, &MainWindow::resign);

	// Settings menu
	settingsMenu = menuBar()->addMenu("*");
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

	settingsMenu->setTitle(tr("Settings"));
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
	QSettings settings;
	settings.setValue("maingeometry", saveGeometry());
//	settings.setValue("hgeometry", hSplitter->saveState());
//	settings.setValue("vgeometry", vSplitter->saveState());
	settings.setValue("language", locale);
	settings.setValue("player", gameSetting.player);
}

void MainWindow::readSettings()
{
	QSettings settings;
	QByteArray maingeometry = settings.value("maingeometry", QByteArray()).toByteArray();
//	QByteArray hgeometry = settings.value("hgeometry", QByteArray()).toByteArray();
//	QByteArray vgeometry = settings.value("vgeometry", QByteArray()).toByteArray();
	locale = settings.value("language", QString()).toString();
	gameSetting.player = settings.value("player", QString()).toString();
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
	playEngine->unload();
	NewGameDialog dialog(this);
	dialog.setDefault(gameSetting,engines);
	if (dialog.exec() == QDialog::Rejected)
		return;
	gameSetting=dialog.getSetting();
	currentGame->newGame();
	boardwindow->setPosition(currentGame->getPosition().board());
	int color = gameSetting.color;
	if (color == 2)
		color = QRandomGenerator::global()->bounded(0, 1);
	if (color == WHITE)
	{
		currentGame->white(gameSetting.player);
		currentGame->black(gameSetting.computer);
	}
	else
	{
		currentGame->white(gameSetting.computer);
		currentGame->black(gameSetting.player);
	}
	engineColor = OTHERPLAYER(color);
	currentGame->date(QDate().currentDate().toString("YYYY.MM.DD"));
	running = true;
	clockwindow->settime(gameSetting.startTime*1000, gameSetting.startTime*1000);
	clockwindow->start(currentGame->getPosition().board().toMove);
	scoresheet->updateGame(currentGame);
//	playEngine->load(eng);
	QString setup = "setoption name Personality value ";
	setup += gameSetting.computer;
	setup += "\n";
	if (engineColor == WHITE)
	{
		int mtg=0;
		if (gameSetting.suddenDeathTime)
		{
			mtg = 40 - currentGame->moveCount(WHITE);
		}
		ChessBoard b = currentGame->getStartPosition().board();
		MoveList ml = currentGame->movelist();

		playEngine->search(b, ml,NORMAL_SEARCH, gameSetting.startTime * 1000, gameSetting.startTimeInc * 1000, gameSetting.startTime * 1000, gameSetting.startTimeInc * 1000, mtg);
		boardwindow->flip(true);
	}
	playEngine->loadSetup(setup);
	
}

void MainWindow::playEngineMessage(const QString& msg)
{
	statusBar()->showMessage(msg);
}

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
	currentGame->addComment(tr("Lost on time."));
	if (color == WHITE)
		currentGame->result(QString("0-1"));
	else
		currentGame->result(QString("1-0"));
	endGame();
}

void MainWindow::moveEntered(ChessMove& move)
{
	int tomove = currentGame->toMove();

	// Move not allowed, engine to move.
	if (running && (tomove == engineColor))
	{
		QChessPosition pos = currentGame->getPosition();
		boardwindow->setPosition(pos.board());
		return;
	}

	// Do the move if it is legal
	if (!currentGame->doMove(move))
	{
		QChessPosition pos = currentGame->getPosition();
		boardwindow->setPosition(pos.board());
		return;
	}

	scoresheet->updateGame(currentGame);

	if (!running)
		return;

	// Update clock if needed
	if (gameSetting.startTimeInc)
		clockwindow->addtime(gameSetting.startTimeInc * 1000, tomove);
	if (gameSetting.suddenDeathTime && currentGame->moveCount(tomove) == 40)
		clockwindow->addtime(gameSetting.suddenDeathTime * 1000, tomove);

	// Switch the clock
	clockwindow->start(currentGame->toMove());

	// Let the engine find a move
	int mtg = 0;
	if (gameSetting.suddenDeathTime)
	{
		mtg = 40 - currentGame->moveCount(currentGame->toMove());
		if (mtg < 0)
			mtg = 0;
	}
	ChessBoard b = currentGame->getStartPosition().board();
	MoveList ml = currentGame->movelist();
	playEngine->search(b, ml, NORMAL_SEARCH, clockwindow->gettime(WHITE), gameSetting.startTimeInc * 1000, clockwindow->gettime(BLACK), gameSetting.startTimeInc * 1000, mtg);
}

void MainWindow::endGame()
{
	playEngine->unload();
	saveGame();
	scoresheet->updateGame(currentGame);
	running = false;
	clockwindow->stop();
}

void MainWindow::playEngineReady()
{
}

void MainWindow::playEngineMove(const QString& move, const QString& ponder)
{
	if (!running)
		return;
	int player = currentGame->toMove();
	ChessMove m=currentGame->getPosition().board().getMoveFromText(move.toStdString());
	currentGame->doMove(m);
	boardwindow->setPosition(currentGame->getPosition().board());
	scoresheet->updateGame(currentGame);
	if (gameSetting.startTimeInc)
		clockwindow->addtime(gameSetting.startTimeInc * 1000, player);
	if (gameSetting.suddenDeathTime && currentGame->moveCount(player) == 40)
		clockwindow->addtime(gameSetting.suddenDeathTime * 1000, player);

	clockwindow->start(currentGame->toMove());
}

void MainWindow::resign()
{
	// Test
	QSettings settings("Engine.ini", QSettings::IniFormat);
	settings.beginGroup("PolarChess");
	settings.setValue("path", "Engine.exe");
	settings.setValue("book", "Polarchess");
	settings.setValue("bookdepth", 10);
	settings.setValue("Elo", 1600);
	settings.beginWriteArray("option");
	settings.setValue("UCI_LimitStrength", true);
	settings.setValue("UCI_Elo", 1600);
	settings.endArray();
	settings.endGroup();

	if (!running)
		return;
	if (engineColor==WHITE)
		currentGame->result(QString("1-0"));
	else
		currentGame->result(QString("0-1"));
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
	playEngine->unload();
	running = false;
	clockwindow->stop();
}

void MainWindow::saveGame()
{
	database->addGame(currentGame);
}