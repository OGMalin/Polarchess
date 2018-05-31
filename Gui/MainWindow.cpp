#include "MainWindow.h"
#include "AboutDialog.h"
#include "NewGameDialog.h"
#include "BoardWindow.h"
#include "Scoresheet.h"
#include "ClockWindow.h"
#include "EngineWindow.h"
#include "Engine.h"
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

MainWindow::MainWindow()
{
	readSettings();

	createMenu();

	statusBar();
	loadLanguage();

//	this->setStyleSheet("background-color:yellow;");

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
	vSplitter->addWidget(enginewindow);


	hSplitter->setStretchFactor(0, 1);
	hSplitter->setStretchFactor(1, 1);
	vSplitter->setStretchFactor(0, 1);
	vSplitter->setStretchFactor(1, 1);
	vSplitter->setStretchFactor(2, 1);

	setCentralWidget(hSplitter);

	retranslateUi();

	playEngine = new Engine();

	connect(playEngine, SIGNAL(engineMessage(const QString&)), this, SLOT(slotEngineMessage(const QString&)));
}

MainWindow::~MainWindow()
{
	/*
	delete boardwindow;
	delete scoresheet;
	delete langGroup;
*/
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
	newGameAct = gameMenu->addAction(QIcon(":/icon/board24.png"),"*",this,&MainWindow::newGame);

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
}

void MainWindow::retranslateUi()
{
	fileMenu->setTitle(tr("File"));
	exitAct->setText(tr("Exit"));

	boardMenu->setTitle(tr("Board"));
	flipAct->setText(tr("Flip board"));

	gameMenu->setTitle(tr("Game"));
	newGameAct->setText(tr("New game"));

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
	QSettings settings; (QCoreApplication::organizationName(), QCoreApplication::applicationName());
	settings.setValue("maingeometry", saveGeometry());
//	settings.setValue("hgeometry", hSplitter->saveState());
//	settings.setValue("vgeometry", vSplitter->saveState());
	settings.setValue("language", locale);
}

void MainWindow::readSettings()
{
	QSettings settings;
	QByteArray maingeometry = settings.value("maingeometry", QByteArray()).toByteArray();
//	QByteArray hgeometry = settings.value("hgeometry", QByteArray()).toByteArray();
//	QByteArray vgeometry = settings.value("vgeometry", QByteArray()).toByteArray();
	locale = settings.value("language", QString()).toString();
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
	NewGameDialog dialog(this);
	dialog.setDefault(gameSetting);
	if (dialog.exec() == QDialog::Rejected)
		return;
	gameSetting=dialog.getSetting();
	currentGame.clear();
	boardwindow->newGame();
	/*
	statusBar()->showMessage("Try to start engine.");
	QString name = "Engine.exe";
	QString dir = "c:\\Engines\\Polarchess\\";
	playEngine->setEngine(name, dir);
	playEngine->load();
	*/
}

void MainWindow::slotEngineMessage(const QString& msg)
{
	statusBar()->showMessage(msg);
}

void MainWindow::aboutDialog()
{
	AboutDialog dialog(this);
	dialog.exec();
}