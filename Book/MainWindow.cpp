#include "MainWindow.h"
#include "MoveWindow.h"
#include "OpeningWindow.h"
#include "CommentWindow.h"
#include "PathWindow.h"
#include "EngineWindow.h"
#include "AboutDialog.h"
#include "ImportPgnDialog.h"
#include "../Common/BoardWindow.h"
#include "Path.h"
#include <QMenu>
#include <QToolBar>
#include <QAction>
#include <QMenuBar>
#include <QSettings>
#include <QApplication>
#include <QRect>
#include <QDesktopWidget>
#include <QCloseEvent>
#include <QSplitter>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>
#include <QSplitter>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	QSettings settings;

	dataPath = QStandardPaths::locate(QStandardPaths::DocumentsLocation, QCoreApplication::organizationName(), QStandardPaths::LocateDirectory);
	dataPath += "/" + QCoreApplication::applicationName();
	write = -1;

	createMenu();
	readSettings();

	statusBar();

	hSplitter = new QSplitter(Qt::Horizontal);
	v1Splitter = new QSplitter(Qt::Vertical);
	v2Splitter = new QSplitter(Qt::Vertical);

	boardwindow = new BoardWindow;
	openingwindow = new OpeningWindow;
	movewindow = new MoveWindow;
	commentwindow = new CommentWindow;
	pathwindow = new PathWindow;
	enginewindow = new EngineWindow;

	v1Splitter->addWidget(openingwindow);
	v1Splitter->addWidget(boardwindow);
	v1Splitter->addWidget(enginewindow);
	v2Splitter->addWidget(pathwindow);
	v2Splitter->addWidget(movewindow);
	v2Splitter->addWidget(commentwindow);
	hSplitter->addWidget(v1Splitter);
	hSplitter->addWidget(v2Splitter);

	setCentralWidget(hSplitter);

	Base[THEORY] = new Database("theory");
	Base[REPWHITE] = new Database("white");
	Base[REPBLACK] = new Database("black");
	currentPath = new Path();
	training = new Training();
	training->SetDatabase(WHITE, Base[REPWHITE]);
	training->SetDatabase(BLACK, Base[REPBLACK]);

	enginewindow->setPosition(currentPath->getStartPosition());

	connect(boardwindow, SIGNAL(moveEntered(ChessMove&)), this, SLOT(moveEntered(ChessMove&)));
	connect(movewindow, SIGNAL(moveSelected(int, int)), this, SLOT(moveSelected(int, int)));
	connect(movewindow, SIGNAL(moveDelete(int, int)), this, SLOT(moveDelete(int, int)));
	connect(movewindow, SIGNAL(addMoveComment(int, int, QString&)), this, SLOT(addMoveComment(int, int, QString&)));
	connect(pathwindow, SIGNAL(pathToDB(int)), this, SLOT(pathToDB(int)));
	connect(pathwindow, SIGNAL(pathSelected(int)), this, SLOT(pathSelected(int)));
	connect(commentwindow, SIGNAL(commentChanged(QString&)), this, SLOT(commentChanged(QString&)));

//	boardwindow->setVisible(false);
//	enginewindow->setVisible(false);
//	openingwindow->setVisible(false);
//	movewindow->setVisible(false);
//	commentwindow->setVisible(false);

	restoreGeometry(settings.value("mainWindowGeometry").toByteArray());
	restoreState(settings.value("mainWindowState").toByteArray());
	v1Splitter->restoreState(settings.value("v1State").toByteArray());
	v2Splitter->restoreState(settings.value("v2State").toByteArray());
	hSplitter->restoreState(settings.value("hState").toByteArray());
}

MainWindow::~MainWindow()
{
	delete currentPath;
	delete training;
}

void MainWindow::createMenu()
{
	// File menu
	fileMenu = menuBar()->addMenu("File");
	fileOpenMenu = fileMenu->addMenu("Open book");
	openAct[THEORY] = fileOpenMenu->addAction("Open theory book", this, &MainWindow::fileOpenTheory);
	openAct[REPWHITE] = fileOpenMenu->addAction("Open White repertoire book", this, &MainWindow::fileOpenWhite);
	openAct[REPBLACK] = fileOpenMenu->addAction("Open Black repertoire book", this, &MainWindow::fileOpenBlack);
	fileNewMenu = fileMenu->addMenu("New book");
	newAct[THEORY] = fileNewMenu->addAction("New theory book", this, &MainWindow::fileNewTheory);
	newAct[REPWHITE] = fileNewMenu->addAction("New White repertoire book", this, &MainWindow::fileNewWhite);
	newAct[REPBLACK] = fileNewMenu->addAction("New Black repertoire book", this, &MainWindow::fileNewBlack);
	fileCloseMenu = fileMenu->addMenu("Close book");
	closeAct[THEORY] = fileCloseMenu->addAction("Close theory book", this, &MainWindow::fileCloseTheory);
	closeAct[REPWHITE] = fileCloseMenu->addAction("Close White repertoire book", this, &MainWindow::fileCloseWhite);
	closeAct[REPBLACK] = fileCloseMenu->addAction("Close Black repertoire book", this, &MainWindow::fileCloseBlack);
	fileImportMenu = fileMenu->addMenu("Import");
	importPgnAct = fileImportMenu->addAction("Import pgnfiles", this, &MainWindow::fileImportPgn);
	importBookAct = fileImportMenu->addAction("Import book", this, &MainWindow::fileImportBook);
	createStatAct = fileImportMenu->addAction("Import Statistics", this, &MainWindow::fileCreateStatistics);
	fileMenu->addSeparator();
	exitAct = fileMenu->addAction("Exit", this, &QWidget::close);

	bookMenu = menuBar()->addMenu("Book");
	bookWriteMenu = bookMenu->addMenu("Write enable");
	writeAct[THEORY] = bookWriteMenu->addAction("Write to theory book", this, &MainWindow::bookWriteTheory);
	writeAct[REPWHITE] = bookWriteMenu->addAction("Write to White repertoire book", this, &MainWindow::bookWriteWhite);
	writeAct[REPBLACK] = bookWriteMenu->addAction("Write to Black repertoire book", this, &MainWindow::bookWriteBlack);
	bookMenu->addSeparator();

	trainingMenu = menuBar()->addMenu("Training");
	clearTrainingAct = trainingMenu->addAction("Clear trainingdata", this, &MainWindow::trainingClearData);
	createTrainingFullAct = trainingMenu->addAction("Create training from startposition", this, &MainWindow::trainingCreateFull);
	createTrainingPosAct = trainingMenu->addAction("Create training from current position", this, &MainWindow::trainingCreateFromPos);

	// Setting up the toolbar
	toolbar = addToolBar("Toolbar");
	for (int i = 0; i < 3; i++)
	{
		toolbar->addAction(writeAct[i]);
		writeAct[i]->setCheckable(true);
		closeAct[i]->setDisabled(true);
		writeAct[i]->setDisabled(true);
	}

	// No database opened as default
	importPgnAct->setDisabled(true);

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
			closeAct[i]->setDisabled(false);
			writeAct[i]->setDisabled(false);
			++open;
		}else
		{
			closeAct[i]->setDisabled(true);
			writeAct[i]->setDisabled(true);
		}
	}
	if (open==0)
		importPgnAct->setDisabled(true);
	else
		importPgnAct->setDisabled(false);
}

void MainWindow::writeSettings()
{/*
	QSettings settings;
	settings.setValue("maingeometry", saveGeometry());
	*/
}

void MainWindow::readSettings()
{
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
	QSettings settings;
	settings.setValue("mainWindowGeometry", saveGeometry());
	settings.setValue("mainWindowState", saveState());
	settings.setValue("v1State", v1Splitter->saveState());
	settings.setValue("v2State", v2Splitter->saveState());
	settings.setValue("hState", hSplitter->saveState());
	//	writeSettings();
//	event->accept();
}

void MainWindow::fileOpen(int type)
{
	QMessageBox msgbox;
	QString path = QFileDialog::getOpenFileName(this, "Open book", dataPath, "Book files (*.book)");
	if (!path.isEmpty())
	{
		if (!Base[type]->open(path))
		{
			msgbox.setText("Can't open book");
			msgbox.exec();
			return;
		}
		bde[type] = Base[type]->find(currentPath->getPosition());

		movewindow->update(bde[THEORY], bde[REPWHITE], bde[REPBLACK]);
//		openingwindow->update(bdeTheory, bdeRep);
		commentwindow->update(bde[THEORY].comment, bde[REPWHITE].comment, bde[REPBLACK].comment);
/*
		movewindow->setVisible(true);
		commentwindow->setVisible(true);
		*/

		if (write == type)
		{
			write = -1;
		}
		updateMenu();
	}
}

void MainWindow::fileNew(int type)
{
	QString path = QFileDialog::getSaveFileName(this, "Open book", dataPath, "Book files (*.book)");
	if (!path.isEmpty())
	{
		QFile file(path);
		if (file.exists())
		{
			QMessageBox msgbox;
			msgbox.setText("The book allready exist. Do you want to delete it?");
			msgbox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
			if (msgbox.exec() != QMessageBox::Ok)
				return;
			file.remove();
		}
		Base[type]->create(path, type);
		bde[type].clear();
		bde[type].board = currentPath->getStartPosition();
		Base[type]->add(bde[type]);

		bde[type] = Base[type]->find(currentPath->getPosition());
		
		movewindow->update(bde[THEORY], bde[REPWHITE], bde[REPBLACK]);
//		openingwindow->update(bdeTheory, bdeRep);
		commentwindow->update(bde[THEORY].comment, bde[REPWHITE].comment, bde[REPBLACK].comment);
		/*
		movewindow->setVisible(true);
		commentwindow->setVisible(true);
		*/

		if (write == type)
			write = -1;
		updateMenu();
	}
}

void MainWindow::fileClose(int type)
{
	Base[type]->close();
	bde[type].clear();

	if (write == type)
		write = -1;

	updateMenu();
	movewindow->update(bde[THEORY], bde[REPWHITE], bde[REPBLACK]);
	commentwindow->update(bde[THEORY].comment, bde[REPWHITE].comment, bde[REPBLACK].comment);
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

void MainWindow::moveSelected(int rep, int movenr)
{
	if ((rep < 0) || (rep > 2))
		return;
	if (bde[rep].movelist.size() <= movenr)
		return;
	ChessBoard board = currentPath->getPosition();
	int i;

	// Do the move if it is legal
	if (!currentPath->add(bde[rep].movelist[movenr].move))
		return;

	// Change to read from both db
	board = currentPath->getPosition();

	for (int i = 0; i < 3; i++)
	{
		bde[i] = Base[i]->find(board);
	}
	boardwindow->setPosition(board);
	enginewindow->setPosition(board, currentPath->size() / 2 + 1);
	movewindow->update(bde[THEORY], bde[REPWHITE], bde[REPBLACK]);
	//	openingwindow->update(bdeTheory, bdeRep);
	commentwindow->update(bde[THEORY].comment, bde[REPWHITE].comment, bde[REPBLACK].comment);
	pathwindow->update(currentPath);
}

void MainWindow::moveDelete(int rep, int movenr)
{
	if ((rep < 0) || (rep > 2))
		return;
	if (bde[rep].movelist.size() <= movenr)
		return;
	bde[rep].movelist.removeAt(movenr);
	Base[rep]->add(bde[rep]);
	movewindow->update(bde[THEORY], bde[REPWHITE], bde[REPBLACK]);
	commentwindow->update(bde[THEORY].comment, bde[REPWHITE].comment, bde[REPBLACK].comment);
}

void MainWindow::addMoveComment(int rep, int movenr, QString& comment)
{
	if ((rep < 0) || (rep > 2))
		return;
	if (bde[rep].movelist.size() <= movenr)
		return;
	bde[rep].movelist[movenr].comment = comment;
	Base[rep]->add(bde[rep]);
	movewindow->update(bde[THEORY], bde[REPWHITE], bde[REPBLACK]);
	commentwindow->update(bde[THEORY].comment, bde[REPWHITE].comment, bde[REPBLACK].comment);
}

void MainWindow::moveEntered(ChessMove& move)
{
	ChessBoard board = currentPath->getPosition();
	BookDBMove bm;
	int i;

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

	for (int i = 0; i < 3; i++)
	{
		bde[i] = Base[i]->find(board);
	}
	boardwindow->setPosition(board);
	enginewindow->setPosition(board,currentPath->size()/2+1);
	movewindow->update(bde[THEORY], bde[REPWHITE],bde[REPBLACK]);
//	openingwindow->update(bdeTheory, bdeRep);
	commentwindow->update(bde[THEORY].comment, bde[REPWHITE].comment, bde[REPBLACK].comment);
	pathwindow->update(currentPath);
}

void MainWindow::pathSelected(int ply)
{
	if (ply < 1)
		currentPath->clear();
	else
		currentPath->setLength(ply);

	ChessBoard board = currentPath->getPosition();

	for (int i = 0; i < 3; i++)
		bde[i] = Base[i]->find(board);
	boardwindow->setPosition(board);
	enginewindow->setPosition(board, currentPath->size() / 2 + 1);
	movewindow->update(bde[THEORY], bde[REPWHITE], bde[REPBLACK]);
//	openingwindow->update(bdeTheory, bdeRep);
	commentwindow->update(bde[THEORY].comment, bde[REPWHITE].comment, bde[REPBLACK].comment);
	pathwindow->update(currentPath);
}

void MainWindow::pathToDB(int rep)
{
	if (Base[rep]->isOpen())
	{
		BookDBEntry bd;
		PathEntry pe;
		BookDBMove bm;
		for (int i = 0; i < currentPath->size(); i++)
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
	int moves=999;
	ImportPgnDialog dialog(this);
	dialog.setItems(Base[THEORY]->isOpen(), Base[REPWHITE]->isOpen(), Base[REPBLACK]->isOpen(), moves);
	if (dialog.exec() == QDialog::Rejected)
		return;
	bool theory, white, black, variation, comment;
	QString path;
	dialog.getItems(path, theory, white, black, moves, comment, variation);

	dialog.importPgnFile(this, Base[THEORY], path, moves, comment, variation);

	// Change to read from both db
	ChessBoard board = currentPath->getPosition();
	for (int i = 0; i < 3; i++)
		bde[i] = Base[i]->find(board);
	boardwindow->setPosition(board);
	movewindow->update(bde[THEORY], bde[REPWHITE], bde[REPBLACK]);
	//	openingwindow->update(bdeTheory, bdeRep);
	commentwindow->update(bde[THEORY].comment, bde[REPWHITE].comment, bde[REPBLACK].comment);
}

void MainWindow::fileImportBook()
{
	Database iBase("ImportBase");
	QMessageBox msgbox;
	QString path = QFileDialog::getOpenFileName(this, "Open book to import", dataPath, "Book files (*.book)");
	if (!path.isEmpty())
	{
		if (!iBase.open(path))
		{
			msgbox.setText("Can't open book to import");
			msgbox.exec();
			return;
		}
		msgbox.setText("Select book to import to");
		QPushButton *theoryButton = msgbox.addButton("Theory", QMessageBox::ActionRole);
		QPushButton *whiteButton = msgbox.addButton("White repertoire", QMessageBox::ActionRole);
		QPushButton *blackButton = msgbox.addButton("Black repertoire", QMessageBox::ActionRole);
		QPushButton *abortButton = msgbox.addButton(QMessageBox::Abort);
		if (!Base[THEORY]->isOpen())
			theoryButton->setDisabled(true);
		if (!Base[REPWHITE]->isOpen())
			whiteButton->setDisabled(true);
		if (!Base[REPBLACK]->isOpen())
			blackButton->setDisabled(true);
		msgbox.exec();
		if (msgbox.clickedButton() == theoryButton)
			Base[THEORY]->importBase(&iBase);
		else if (msgbox.clickedButton() == whiteButton)
			Base[REPWHITE]->importBase(&iBase);
		else if (msgbox.clickedButton() == blackButton)
			Base[REPBLACK]->importBase(&iBase);
		else
			return;

		ChessBoard board = currentPath->getPosition();
		for (int i = 0; i < 3; i++)
			bde[i] = Base[i]->find(board);
		movewindow->update(bde[THEORY], bde[REPWHITE], bde[REPBLACK]);
		commentwindow->update(bde[THEORY].comment, bde[REPWHITE].comment, bde[REPBLACK].comment);
	}
}

void MainWindow::fileCreateStatistics()
{
	movewindow->createStatistics();
}

void MainWindow::trainingClearData()
{
	Base[REPWHITE]->clearAllTrainingData();
	Base[REPBLACK]->clearAllTrainingData();
}

void MainWindow::trainingCreateFull()
{
	ChessBoard cb;
	cb.startposition();
	training->create(cb);
}

void MainWindow::trainingCreateFromPos()
{
	ChessBoard cb = currentPath->getPosition();
	training->create(cb);
}
