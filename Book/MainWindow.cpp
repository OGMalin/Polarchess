#include "MainWindow.h"
#include "AboutDialog.h"
#include "DatabaseDialog.h"
#include "AnalyzeDialog.h"
#include "CreateEngine.h"
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
#include <QList>
#include <string>
#include <QByteArray>
#include <QDebug>
#include <QMetaType>

Q_DECLARE_METATYPE(QList<int>)

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	// QList<int> must be registred before used as a QVariant for saving to the register (QSettings).
	qRegisterMetaTypeStreamOperators<QList<int> >("QList<int>");

	write = -1;
	inTraining = false;

	createMenu();
	createStatusbar();

	statusBar();

	hSplitter = new QSplitter(Qt::Horizontal);
	v1Splitter = new QSplitter(Qt::Vertical);
	v2Splitter = new QSplitter(Qt::Vertical);

	boardwindow = new BoardWindow;
	openingwindow = new OpeningWindow;
	commentwindow = new CommentWindow;
	pathwindow = new PathWindow;
	enginewindow = new EngineWindow;
	movewindow = new MoveTableWindow;
	trainingwindow = new TrainingWindow;
	
	v1Splitter->addWidget(openingwindow);
	v1Splitter->addWidget(boardwindow);
	v1Splitter->addWidget(enginewindow);
	
	v2Splitter->addWidget(pathwindow);
	v2Splitter->addWidget(movewindow);
	v2Splitter->addWidget(commentwindow);
	v2Splitter->addWidget(trainingwindow);

	hSplitter->addWidget(v1Splitter);
	hSplitter->addWidget(v2Splitter);

	setCentralWidget(hSplitter);

	Base[THEORY] = new Database("theory");
	Base[REPWHITE] = new Database("white");
	Base[REPBLACK] = new Database("black");
	statisticsDB = new Statistics();
	computerDB = new Computer();
	openingsDB = new Openings();
	currentPath = new Path();
	trainingDB = new Training();
	trainingDB->SetRepertoireDatabase(WHITE, Base[REPWHITE]);
	trainingDB->SetRepertoireDatabase(BLACK, Base[REPBLACK]);
	trainingwindow->setTrainingDB(trainingDB);
	trainingwindow->setDatabase(Base[THEORY], Base[REPWHITE], Base[REPBLACK], computerDB);
	movewindow->computerDB = computerDB;
	openingwindow->setOpeningsDB(openingsDB);
	dgt = NULL;
	readSettings();

	loadLanguage();
	retranslateUi();

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
	connect(commentwindow, SIGNAL(commentChanged(QString&, int)), this, SLOT(commentChanged(QString&, int)));
	connect(enginewindow, SIGNAL(enginePV(ComputerDBEngine&, ChessBoard&)), this, SLOT(enginePV(ComputerDBEngine&, ChessBoard&)));
	connect(trainingwindow, SIGNAL(trainingFlipBoard(int)), this, SLOT(trainingFlipBoard(int)));
	connect(trainingwindow, SIGNAL(trainingAddMoves(MoveList&)), this, SLOT(trainingAddMoves(MoveList&)));
	connect(trainingwindow, SIGNAL(trainingSetArrow(int, int, bool, int)), this, SLOT(trainingSetArrow(int, int, bool, int)));
	connect(trainingwindow, SIGNAL(trainingStop()), this, SLOT(trainingStop()));

	ChessBoard board = currentPath->getPosition();

	readDB();

	boardwindow->setPosition(board);
	enginewindow->setPosition(board, currentPath->current() / 2 + 1);

	trainingwindow->hide();
	updateMenu();
	updateWindow();
}

MainWindow::~MainWindow()
{
	delete currentPath;
	delete trainingDB;
}

void MainWindow::createMenu()
{
	// File menu
	fileMenu = menuBar()->addMenu("*");
	exitAct = fileMenu->addAction("*", this, &QWidget::close);

	bookMenu = menuBar()->addMenu("Book");
	bookAnalyzeAct = bookMenu->addAction("*", this, &MainWindow::bookAnalyze);
	bookMenu->addSeparator();
	bookWriteMenu = bookMenu->addMenu("Write enable");
	writeAct[THEORY] = bookWriteMenu->addAction("Write to theory book", this, &MainWindow::bookWriteTheory);
	writeAct[REPWHITE] = bookWriteMenu->addAction("Write to White repertoire book", this, &MainWindow::bookWriteWhite);
	writeAct[REPBLACK] = bookWriteMenu->addAction("Write to Black repertoire book", this, &MainWindow::bookWriteBlack);

	trainingMenu = menuBar()->addMenu("*");
	startTrainingAct = trainingMenu->addAction("*", this, &MainWindow::trainingStart);
	stopTrainingAct = trainingMenu->addAction("*", this, &MainWindow::trainingStop);
	bookMenu->addSeparator();
	clearTrainingAct = trainingMenu->addAction("*", this, &MainWindow::trainingClearData);
	createTrainingAct = trainingMenu->addAction("*", this, &MainWindow::trainingCreate);

	settingsMenu = menuBar()->addMenu("*");
	setupDatabaseAct = settingsMenu->addAction("*", this, &MainWindow::setupDatabase);
	createEngineAct = settingsMenu->addAction("*", this, &MainWindow::createEngine);
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
#ifdef _DEBUG
	menuBar()->addAction("Test", this, &MainWindow::test);
#endif
	// Setting up the toolbar
	toolbar = addToolBar("Toolbar");
	toolbar->setObjectName("MainToolbar");
	toolbarTrainingAct = toolbar->addAction(QIcon(":/icon/training.png"),"Training");
	toolbarTrainingAct->setCheckable(true);
	connect(toolbar, SIGNAL(actionTriggered(QAction*)), this, SLOT(toolbarAction(QAction*)));
//	connect(act, toggled(bool),this, &MainWindow::toolbarTraining(bool));
	//for (int i = 0; i < 3; i++)
	//{
	//	toolbar->addAction(writeAct[i]);
	//	writeAct[i]->setCheckable(true);
	//	writeAct[i]->setDisabled(true);
	//}
}

void MainWindow::retranslateUi()
{
	fileMenu->setTitle(tr("File"));
	exitAct->setText(tr("Exit"));

	bookAnalyzeAct->setText(tr("Annalyze book"));
	trainingMenu->setTitle(tr("Training"));
	startTrainingAct->setText(tr("Start training"));
	stopTrainingAct->setText(tr("Stop training"));
	clearTrainingAct->setText(tr("Clear trainingdata"));
	createTrainingAct->setText(tr("Create training"));

	//boardMenu->setTitle(tr("Board"));
	//flipAct->setText(tr("Flip board"));

	//gameMenu->setTitle(tr("Game"));
	//newGameAct->setText(tr("New game"));
	//abortAct->setText(tr("Abort"));
	//resignAct->setText(tr("Resign"));

	settingsMenu->setTitle(tr("Settings"));
	setupDatabaseAct->setText(tr("Setup database"));
	createEngineAct->setText(tr("Create engine"));
	useDgtAct->setText(tr("Use DGT Electronic board"));
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
	if (inTraining)
	{

	}
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

	if (locale == "nb")
		norAct->setChecked(true);
	else
		engAct->setChecked(true);
}

void MainWindow::updateWindow()
{
	pathwindow->refresh(currentPath);

	if(inTraining)
	{
		movewindow->setVisible(false);
		commentwindow->setVisible(false);
		enginewindow->setVisible(false);
		openingwindow->setVisible(false);
		trainingwindow->setVisible(true);
	}
	else
	{
		movewindow->refresh(bde[THEORY], bde[REPWHITE], bde[REPBLACK], sde, cde, currentPath->getPosition(), currentPath->current() / 2 + 1);
		commentwindow->refresh(bde[THEORY].comment, bde[REPWHITE].comment, bde[REPBLACK].comment);
		openingwindow->refresh(currentPath);
		trainingwindow->setVisible(false);
		movewindow->setVisible(true);
		commentwindow->setVisible(true);
		enginewindow->setVisible(true);
		openingwindow->setVisible(true);
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
	sde = statisticsDB->find(cb);
	cde = computerDB->find(cb);

}

void MainWindow::createStatusbar()
{
	statusWatch = new Watch(NULL, true);
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
	if (inTraining)
		trainingSplitterSize = v2Splitter->sizes();
	else
		mainSplitterSize = v2Splitter->sizes();
	if (mainSplitterSize.size())
		settings.setValue("mainSplitterSize", QVariant::fromValue(mainSplitterSize));
	if (trainingSplitterSize.size())
		settings.setValue("trainingSplitterSize", QVariant::fromValue(trainingSplitterSize));

	settings.setValue("dataTheory", Base[THEORY]->getPath());
	settings.setValue("dataWhite", Base[REPWHITE]->getPath());
	settings.setValue("dataBlack", Base[REPBLACK]->getPath());
	settings.setValue("dataStatistics", statisticsDB->getPath());
	settings.setValue("dataComputer", computerDB->getPath());
	settings.setValue("dataTraining", trainingDB->getPath());
	settings.setValue("dataOpenings", openingsDB->getPath());
	settings.setValue("language", locale);
	settings.setValue("movewindowFont", movewindow->fontToString());
	settings.setValue("enginewindowFont", enginewindow->fontToString());
	settings.setValue("pathwindowFont", pathwindow->fontToString());
	settings.setValue("commentwindowFont", commentwindow->fontToString());
	settings.setValue("trainingwindowFont", trainingwindow->fontToString());
	settings.setValue("goodColor", goodColor.name());
	settings.setValue("badColor", badColor.name());
}

void MainWindow::readSettings()
{
	QSettings settings;
	QString dataPath;
	QString dataTheory;
	QString dataWhite;
	QString dataBlack;
	QString dataStatistics;
	QString dataComputer;
	QString dataTraining;
	QString dataOpenings;

	restoreGeometry(settings.value("mainWindowGeometry").toByteArray());
	restoreState(settings.value("mainWindowState").toByteArray());
	v1Splitter->restoreState(settings.value("v1State").toByteArray());
	v2Splitter->restoreState(settings.value("v2State").toByteArray());
	hSplitter->restoreState(settings.value("hState").toByteArray());

	mainSplitterSize = settings.value("mainSplitterSize").value<QList<int> >();
	trainingSplitterSize = settings.value("trainingSplitterSize").value<QList<int> >();
	if (mainSplitterSize.size())
		v2Splitter->setSizes(mainSplitterSize);


	dataPath = settings.value("dataPath",QString()).toString();
	// Set default datapath
	if (dataPath.isEmpty())
	{
		dataPath = QStandardPaths::locate(QStandardPaths::DocumentsLocation, QCoreApplication::organizationName(), QStandardPaths::LocateDirectory);
		dataPath += "/" + QCoreApplication::applicationName();
		settings.setValue("dataPath", dataPath);
	}
	dataTheory = settings.value("dataTheory", dataPath + "/Theory.pbk").toString();
	dataWhite = settings.value("dataWhite", dataPath + "/White.pbk").toString();
	dataBlack = settings.value("dataBlack", dataPath + "/Black.pbk").toString();
	dataStatistics = settings.value("dataStatistics", dataPath + "/Statistics.pst").toString();
	dataComputer = settings.value("dataComputer", dataPath + "/Computer.pcp").toString();
	dataTraining = settings.value("dataTraining", dataPath + "/Training.ptr").toString();
	dataOpenings = settings.value("dataOpenings", dataPath + "/Openings.pop").toString();
	locale = settings.value("language", QString()).toString();
	if (locale.isEmpty())
	{
		// Find the systems default language
		locale = QLocale::system().name();
		locale.truncate(locale.lastIndexOf('_'));
	}
	movewindow->fontFromString(settings.value("movewindowFont", QString()).toString());
	enginewindow->fontFromString(settings.value("enginewindowFont", QString()).toString());
	pathwindow->fontFromString(settings.value("pathwindowFont", QString()).toString());
	commentwindow->fontFromString(settings.value("commentwindowFont", QString()).toString());
	trainingwindow->fontFromString(settings.value("trainingwindowFont", QString()).toString());

	goodColor.setNamedColor(settings.value("goodColor", "#00CC00").toString());
	badColor.setNamedColor(settings.value("badColor", "#CC0000").toString());

	// Open default databases
	if (!dataTheory.isEmpty())
		if (!Base[THEORY]->open(dataTheory))
			Base[THEORY]->create(dataTheory, THEORY);
	if (!dataWhite.isEmpty())
		if (!Base[REPWHITE]->open(dataWhite))
			Base[REPWHITE]->create(dataWhite, REPWHITE);
	if (!dataBlack.isEmpty())
		if (!Base[REPBLACK]->open(dataBlack))
			Base[REPBLACK]->create(dataBlack, REPBLACK);
	if (!dataStatistics.isEmpty())
		if (!statisticsDB->open(dataStatistics))
			statisticsDB->create(dataStatistics);
	if (!dataComputer.isEmpty())
		if (!computerDB->open(dataComputer))
			computerDB->create(dataComputer);
	if (!dataTraining.isEmpty())
		if (!trainingDB->open(dataTraining))
			trainingDB->create(dataTraining);
	if (!dataOpenings.isEmpty())
		if (!openingsDB->open(dataOpenings))
			openingsDB->create(dataOpenings);

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

void MainWindow::bookAnalyze()
{
	disconnect(enginewindow, nullptr, nullptr, nullptr);
	AnalyzeDialog dialog(this, computerDB, Base[THEORY], Base[REPWHITE], Base[REPBLACK], enginewindow, boardwindow, currentPath, trainingDB);
	dialog.exec();
	connect(enginewindow, SIGNAL(enginePV(ComputerDBEngine&, ChessBoard&)), this, SLOT(enginePV(ComputerDBEngine&, ChessBoard&)));
}

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
	if (!currentPath->getPosition().isLegal(move))
	{
		boardwindow->setPosition(currentPath->getPosition());
		return;
	}
		
	if (inTraining)
	{
		if (trainingwindow->isRunning())
			trainingwindow->moveEntered(move);
		else
			boardwindow->setPosition(currentPath->getPosition());
		return;
	}

	// Do the move if it is legal
	if (!currentPath->add(move))
	{
		boardwindow->setPosition(currentPath->getPosition());
		return;
	}


	if (write >= 0)
	{
		// Save the move if it doesn't exist
		if (!bde[write].moveExist(move))
		{
			BookDBMove bm;
			bm.clear();
			bm.move = move;
			bde[write].movelist.append(bm);
			Base[write]->add(bde[write]);
		}
	}

	readDB();

	boardwindow->setPosition(currentPath->getPosition());
	enginewindow->setPosition(currentPath->getPosition(),currentPath->current()/2+1);

	updateWindow();
}

void MainWindow::pathSelected(int ply)
{
	if (inTraining)
		return;

	currentPath->current(ply);

	readDB();

	boardwindow->setPosition(currentPath->getPosition());
	enginewindow->setPosition(currentPath->getPosition(), currentPath->current() / 2 + 1);
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

void MainWindow::commentChanged(QString& comment, int rep)
{
	bde[rep].comment = comment;
	Base[rep]->add(bde[rep]);
}

void MainWindow::trainingStart()
{
	QSettings settings;
	inTraining = true;
	mainSplitterSize = v2Splitter->sizes();
	trainingwindow->setCurrentBoard(currentPath->getPosition());
	trainingwindow->updateStat();
	updateWindow();
	updateMenu();
	if (trainingSplitterSize.size())
		v2Splitter->setSizes(trainingSplitterSize);
}

void MainWindow::trainingStop()
{
	QSettings settings;
	inTraining = false;
	if (trainingwindow->isRunning())
		trainingwindow->stopRunning();
	toolbarTrainingAct->setChecked(false);
	trainingSplitterSize = v2Splitter->sizes();

	readDB();

	updateWindow();
	updateMenu();
	if (mainSplitterSize.size())
		v2Splitter->setSizes(mainSplitterSize);
}

void MainWindow::trainingFlipBoard(int color)
{
	boardwindow->flip(color == BLACK);
}

void MainWindow::trainingAddMoves(MoveList& ml)
{
	int i;
	currentPath->clear();
	for (i = 0; i < ml.size(); i++)
	{
		currentPath->add(ml[i]);
	}
	boardwindow->setPosition(currentPath->getPosition());
	updateWindow();
}

void MainWindow::trainingSetArrow(int fromSq, int toSq, bool wrong, int sek)
{
	boardwindow->markArrow(fromSq, toSq, (wrong ? badColor : goodColor), sek);
}

void MainWindow::trainingClearData()
{
	trainingDB->clearAll();
}

void MainWindow::trainingCreate()
{
	trainingDB->createLines(this);
}

void MainWindow::enginePV(ComputerDBEngine& ce, ChessBoard& cb)
{
	// cb could either be current position or a freezing position.
	computerDB->add(ce, cb);
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
	DatabaseDialog dialog(this, Base[THEORY], Base[REPWHITE], Base[REPBLACK], trainingDB, computerDB, statisticsDB, openingsDB);
	dialog.exec();
	write = -1;
	readDB();
	updateWindow();
	updateMenu();
}

void MainWindow::dgtStatus(int status)
{
	//QList<QToolButton*> list;
	//list = statusBar()->findChildren<QToolButton*>();
	//QList<QToolButton*>::iterator it=list.begin();
	//while (it != list.end())
	//{
	//	++it;
	//}
}

void MainWindow::dgtStatusClicked(bool)
{
	if (dgt->isVisible())
		dgt->hide();
	else
		dgt->show();
}

void MainWindow::toolbarAction(QAction* action)
{
	if (action == toolbarTrainingAct)
	{
		if (action->isChecked())
		{
			if (!inTraining)
				trainingStart();
		}
		else
		{
			if (inTraining)
				trainingStop();
		}
	}
}

void MainWindow::useDgt()
{
	if (useDgtAct->isChecked())
	{
		if (!dgt)
		{
			dgt = new DgtBoard();
			dgtIcon = new QToolButton();
			dgtIcon->setIcon(QIcon(":/icon/dgtDisconnect.png"));
			connect(dgtIcon, SIGNAL(clicked(bool)), this, SLOT(dgtStatusClicked(bool)));
			statusBar()->addPermanentWidget(dgtIcon);
		}
	}
	else
	{
		if (dgt)
		{
			statusBar()->removeWidget(dgtIcon);
			disconnect(dgtIcon);
			delete dgt;
			delete dgtIcon;
			dgt = NULL;
		}
	}
}

void MainWindow::createEngine()
{
	CreateEngine dialog(this);
	dialog.exec();
}

#ifdef _DEBUG
void MainWindow::test()
{
	// ** Test av CompressedBoard
	//Pgn pgn;
	//ChessGame game;
	//ChessBoard cb;
	//QByteArray ba;
	//pgn.open("c:/temp/test.pgn", true);
	//int index = 1;
	//int posindex;
	//while (pgn.read(game, index))
	//{
	//	for (posindex=0;posindex<game.position.size();posindex++)
	//	{
	//		ba = CompressedBoard::compress(game.position[posindex].board);
	//		cb = CompressedBoard::decompress(ba);
	//		if (cb != game.position[posindex].board)
	//		{
	//			qDebug() << "Error in game " << index << ", position " << posindex;
	//			qDebug() << game.position[posindex].board.getFen().c_str();
	//			qDebug() << cb.getFen().c_str();
	//		}
	//	}
	//	++index;
	//}

	//** Converting database
	//Base[0]->convertBase();
	//Base[1]->convertBase();
	//Base[2]->convertBase();
}
#endif