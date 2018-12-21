#include "MainWindow.h"
#include "MoveWindow.h"
#include "OpeningWindow.h"
#include "CommentWindow.h"
#include "PathWindow.h"
#include "EngineWindow.h"
#include "AboutDialog.h"
#include "../Common/BoardWindow.h"
#include "../Common/QChessGame.h"
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

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	writeTheory = false;
	writeRep = false;
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

	theoryBase = new Database(QString("theory"));
	repBase = new Database(QString("rep"));
	currentGame = new QChessGame();
	connect(boardwindow, SIGNAL(moveEntered(ChessMove&)), this, SLOT(moveEntered(ChessMove&)));

	enginewindow->setVisible(false);
	openingwindow->setVisible(false);
//	movewindow->setVisible(false);
//	commentwindow->setVisible(false);
}

MainWindow::~MainWindow()
{
	delete currentGame;
}

void MainWindow::createMenu()
{
	// File menu
	fileMenu = menuBar()->addMenu("File");
	fileOpenMenu = fileMenu->addMenu("Open book");
	openTheoryAct = fileOpenMenu->addAction("Open theory book", this, &MainWindow::fileOpenTheory);
	openRepAct = fileOpenMenu->addAction("Open repertoire book", this, &MainWindow::fileOpenRep);
	fileNewMenu = fileMenu->addMenu("New book");
	newTheoryAct = fileNewMenu->addAction("New theory book", this, &MainWindow::fileNewTheory);
	newRepAct = fileNewMenu->addAction("New repertoire book", this, &MainWindow::fileNewRep);
	fileCloseMenu = fileMenu->addMenu("Close book");
	closeTheoryAct = fileCloseMenu->addAction("Close theory book", this, &MainWindow::fileCloseTheory);
	closeRepAct = fileCloseMenu->addAction("Close repertoire book", this, &MainWindow::fileCloseRep);
	fileMenu->addSeparator();
	exitAct = fileMenu->addAction("Exit", this, &QWidget::close);

	bookMenu = menuBar()->addMenu("Book");
	bookWriteMenu = bookMenu->addMenu("Write enable");
	writeTheoryAct = bookWriteMenu->addAction("Write to theory book", this, &MainWindow::bookWriteTheory);
	writeRepAct = bookWriteMenu->addAction("Write to repertoire book", this, &MainWindow::bookWriteRep);
	writeTheoryAct->setCheckable(true);
	writeRepAct->setCheckable(true);

	// Setting up the toolbar
	toolbar = addToolBar("Toolbar");
	toolbar->addAction(writeTheoryAct);
	toolbar->addAction(writeRepAct);

	// No database opened as default
	closeTheoryAct->setDisabled(true);
	closeRepAct->setDisabled(true);
	writeTheoryAct->setDisabled(true);
	writeRepAct->setDisabled(true);
}

void MainWindow::writeSettings()
{
	QSettings settings;
	settings.setValue("maingeometry", saveGeometry());
}

void MainWindow::readSettings()
{
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

}

void MainWindow::closeEvent(QCloseEvent* event)
{
	writeSettings();
	event->accept();
}

void MainWindow::fileOpenTheory()
{
	QMessageBox msgbox;
	QString path = QStandardPaths::locate(QStandardPaths::DocumentsLocation, "/Polarchess", QStandardPaths::LocateDirectory);
	path = QFileDialog::getOpenFileName(this, "Open book", path, "Book files (*.book)");
	if (!path.isEmpty())
	{
		if (!theoryBase->open(path))
		{
			msgbox.setText("Can't open book");
			msgbox.exec();
			return;
		}
		currentGame->newGame();
		bdeTheory = theoryBase->find(currentGame->getStartPosition().board());
		boardwindow->setPosition(currentGame->getStartPosition().board());
		movewindow->update(bdeTheory, bdeRep);
		openingwindow->update(bdeTheory, bdeRep);
		commentwindow->update(bdeTheory, bdeRep);

		movewindow->setVisible(true);
		commentwindow->setVisible(true);

		closeTheoryAct->setDisabled(false);
		writeTheoryAct->setDisabled(false);
	}
}

void MainWindow::fileOpenRep()
{
	QMessageBox msgbox;
	QString path = QStandardPaths::locate(QStandardPaths::DocumentsLocation, "/Polarchess", QStandardPaths::LocateDirectory);
	path = QFileDialog::getOpenFileName(this, "Open book", path, "Book files (*.book)");
	if (!path.isEmpty())
	{
		if (!repBase->open(path))
		{
			msgbox.setText("Can't open book");
			msgbox.exec();
			return;
		}
		currentGame->newGame();
		bdeRep = repBase->find(currentGame->getStartPosition().board());
		boardwindow->setPosition(currentGame->getStartPosition().board());
		movewindow->update(bdeTheory, bdeRep);
		openingwindow->update(bdeTheory, bdeRep);
		commentwindow->update(bdeTheory, bdeRep);

		movewindow->setVisible(true);
		commentwindow->setVisible(true);

		closeRepAct->setDisabled(false);
		writeRepAct->setDisabled(false);
	}
}

void MainWindow::fileNewTheory()
{
	QString path = QStandardPaths::locate(QStandardPaths::DocumentsLocation, "/Polarchess", QStandardPaths::LocateDirectory);
	path = QFileDialog::getSaveFileName(this, "Create book", path, "Book files (*.book)");
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
		theoryBase->create(path);
		currentGame->newGame();
		bdeTheory.clear();
		bdeTheory.board=currentGame->getStartPosition().board();
		bdeTheory.eco = "A00";
		theoryBase->add(bdeTheory);
		boardwindow->setPosition(currentGame->getStartPosition().board());
		movewindow->update(bdeTheory, bdeRep);
		openingwindow->update(bdeTheory, bdeRep);
		commentwindow->update(bdeTheory, bdeRep);

		movewindow->setVisible(true);
		commentwindow->setVisible(true);

		closeTheoryAct->setDisabled(false);
		writeTheoryAct->setDisabled(false);
	}
}

void MainWindow::fileNewRep()
{
	QString path = QStandardPaths::locate(QStandardPaths::DocumentsLocation, "/Polarchess", QStandardPaths::LocateDirectory);
	path = QFileDialog::getSaveFileName(this, "Create book", path, "Book files (*.book)");
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
		repBase->create(path);
		currentGame->newGame();
		bdeRep.clear();
		bdeRep.board = currentGame->getStartPosition().board();
		bdeRep.eco = "A00";
		repBase->add(bdeRep);
		boardwindow->setPosition(currentGame->getStartPosition().board());
		movewindow->update(bdeTheory, bdeRep);
		openingwindow->update(bdeTheory, bdeRep);
		commentwindow->update(bdeTheory, bdeRep);

		movewindow->setVisible(true);
		commentwindow->setVisible(true);

		closeRepAct->setDisabled(false);
		writeRepAct->setDisabled(false);
	}
}

void MainWindow::fileCloseTheory()
{
	bdeTheory.clear();
	closeTheoryAct->setDisabled(true);
	writeTheoryAct->setDisabled(true);

	writeTheory = false;
	writeTheoryAct->setChecked(false);
}

void MainWindow::fileCloseRep()
{
	bdeRep.clear();
	closeRepAct->setDisabled(true);
	writeRepAct->setDisabled(true);

	writeRep = false;
	writeRepAct->setChecked(false);
}

void MainWindow::bookWriteTheory()
{
	writeTheory = writeTheory ? false : true;
	writeTheoryAct->setChecked(writeTheory);
	writeRep = false;
	writeRepAct->setChecked(false);
}

void MainWindow::bookWriteRep()
{
	writeRep = writeRep ? false : true;
	writeRepAct->setChecked(writeRep);
	writeTheory = false;
	writeTheoryAct->setChecked(false);
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

void MainWindow::moveEntered(ChessMove& move)
{
	int tomove = currentGame->toMove();
	QChessPosition pos = currentGame->getPosition();
	BookDBMove bm;
	// Do the move if it is legal
	if (!currentGame->doMove(move))
	{
		boardwindow->setPosition(pos.board());
		return;
	}

	// Save the move if it doesn't exist
	if (writeTheory)
	{
		if (!bdeTheory.moveExist(move))
		{
			bm.move = move;
			bm.score = 0;
			bm.repertoire = 0;
			bdeTheory.movelist.append(bm);
			theoryBase->add(bdeTheory);
		}
	} else if (writeRep)
	{
		if (!bdeRep.moveExist(move))
		{
			bm.move = move;
			bm.score = 0;
			bm.repertoire = 0;
			bdeRep.movelist.append(bm);
			repBase->add(bdeRep);
		}
	}
	// Change to read from both db
	ChessBoard board = currentGame->getPosition().board();
	bdeTheory = theoryBase->find(board);
	bdeRep = repBase->find(board);
	boardwindow->setPosition(board);
	movewindow->update(bdeTheory, bdeRep);
	openingwindow->update(bdeTheory, bdeRep);
	commentwindow->update(bdeTheory, bdeRep);
}
