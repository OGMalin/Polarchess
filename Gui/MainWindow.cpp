#include "MainWindow.h"
#include <QIcon>

MainWindow::MainWindow()
{
	// Find the systems default language
	locale = QLocale::system().name();
	locale.truncate(locale.lastIndexOf('_'));

	createMenu();

	statusBar();
	loadLanguage(locale);

//	this->setStyleSheet("background-color:yellow;");

	QSplitter* hSplitter = new QSplitter(Qt::Horizontal);
	QSplitter* vSplitter = new QSplitter(Qt::Vertical);

	boardwindow = new BoardWindow;
	scoresheet = new Scoresheet;
	clockwindow = new ClockWindow;

	hSplitter->addWidget(boardwindow);
	hSplitter->addWidget(vSplitter);
	vSplitter->addWidget(clockwindow);
	vSplitter->addWidget(scoresheet);


	hSplitter->setStretchFactor(0, 1);
	hSplitter->setStretchFactor(1, 1);
	vSplitter->setStretchFactor(0, 1);
	vSplitter->setStretchFactor(1, 1);

	setCentralWidget(hSplitter);

	retranslateUi();
}

MainWindow::~MainWindow()
{
	/*
	delete boardwindow;
	delete scoresheet;
	delete langGroup;
*/
}

// The text in the menu are set in retranslateUi to be able to switch language 'on the fly'.
void MainWindow::createMenu()
{
	//Main menues
	fileMenu = menuBar()->addMenu("*");
	gameMenu = menuBar()->addMenu("*");
	settingsMenu = menuBar()->addMenu("*");
	helpMenu = menuBar()->addMenu("*");

	// File menu
//	fileMenu->addSeparator();
	exitAct = fileMenu->addAction("*", this, &QWidget::close);

	newGameAct = gameMenu->addAction(QIcon(":/icon/board24.png"),"*");

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

	// Setting up the toolbar
	toolbar=addToolBar("Toolbar");
	toolbar->addAction(newGameAct);

}

void MainWindow::retranslateUi()
{
	fileMenu->setTitle(tr("File"));
	exitAct->setText(tr("Exit"));
	exitAct->setStatusTip(tr("Exit the application"));

	gameMenu->setTitle(tr("Game"));
	newGameAct->setText(tr("New game"));

	settingsMenu->setTitle(tr("Settings"));
	langMenu->setTitle(tr("Language"));
	engAct->setText(tr("English"));
	norAct->setText(tr("Norwegian"));

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
		loadLanguage(action->data().toString());
	}
}

void MainWindow::loadLanguage(const QString& lang)
{
	if (lang == "nb")
	{
		if (translator.isEmpty())
		{
			if (translator.load("gui_nb.qm"))
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

