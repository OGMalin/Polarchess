#include "MainWindow.h"
#include <QIcon>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	// format systems language
	locale = QLocale::system().name();
	locale.truncate(locale.lastIndexOf('_'));
	createMenu();
	createStatusBar();
	loadLanguage(locale);

//	this->setStyleSheet("background-color:yellow;");

//	hSplitter = new QSplitter(this);

	boardwindow = new BoardWindow;
	scoresheet = new Scoresheet(this);
/*
	hSplitter->addWidget(boardwindow);
	hSplitter->addWidget(scoresheet);
	boardwindow->show();
	scoresheet->show();
	hSplitter->setStretchFactor(1, 1);*/
	setCentralWidget(boardwindow);
//	hSplitter->show();
	retranslateUi();
}

MainWindow::~MainWindow()
{
	/*
	delete boardwindow;
	delete scoresheet;
	delete hSplitter;
	delete langGroup;
*/
}

void MainWindow::createMenu()
{
	fileMenu = menuBar()->addMenu("*");
	settingsMenu = menuBar()->addMenu("*");
	helpMenu = menuBar()->addMenu("*");

	// File menu
//	fileMenu->addSeparator();
	exitAct = fileMenu->addAction("*", this, &QWidget::close);

	// Settings menu
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

	// Help menu
	aboutAct = helpMenu->addAction("*");
}

void MainWindow::createStatusBar()
{
	statusBar()->showMessage(tr("Ready"));
}

void MainWindow::setLanguage()
{
	this->repaint();
}

void MainWindow::slotLanguageChanged(QAction* action)
{
	if (0 != action) {
		// load the language dependant on the action content
		loadLanguage(action->data().toString());
	}
}

void MainWindow::loadLanguage(const QString& lang)
{
	if (lang == "gb")
	{
		if (!translator.isEmpty())
			qApp->removeTranslator(&translator);
		return;

	}
	if (translator.isEmpty())
	{
		if (translator.load("gui_nb.qm"))
			qApp->installTranslator(&translator);
	}
	else
	{
		qApp->installTranslator(&translator);
	}
}

void MainWindow::changeEvent(QEvent* event)
{
	if (0 != event) {
		switch (event->type()) {
			// this event is send if a translator is loaded
		case QEvent::LanguageChange:
			menuBar()->repaint();
			retranslateUi();
			break;

			// this event is send, if the system, language changes
		case QEvent::LocaleChange:
		{
			QString lang = QLocale::system().name();
			lang.truncate(lang.lastIndexOf('_'));
			loadLanguage(lang);
		}
		break;
		}
	}
	QMainWindow::changeEvent(event);
}

void MainWindow::retranslateUi()
{
	fileMenu->setTitle(tr("File"));
	settingsMenu->setTitle(tr("Settings"));
	helpMenu->setTitle(tr("Help"));
	langMenu->setTitle(tr("Language"));

	exitAct->setText(tr("Exit"));
	exitAct->setStatusTip(tr("Exit the application"));
	engAct->setText(tr("English"));
	norAct->setText(tr("Norwegian"));
	aboutAct->setText(tr("About..."));
}