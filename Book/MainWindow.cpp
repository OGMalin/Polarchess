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

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	QSettings settings;
	restoreGeometry(settings.value("mainWindowGeometry").toByteArray());

	dataPath = QStandardPaths::locate(QStandardPaths::DocumentsLocation, QCoreApplication::organizationName(), QStandardPaths::LocateDirectory);
	dataPath += "/" + QCoreApplication::applicationName();
	writeTheory = false;
	writeRep = false;
	whiteRep=true;

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
	currentPath = new Path();

	enginewindow->setPosition(currentPath->getStartPosition());

	connect(boardwindow, SIGNAL(moveEntered(ChessMove&)), this, SLOT(moveEntered(ChessMove&)));
	connect(pathwindow, SIGNAL(pathSelected(int)), this, SLOT(pathSelected(int)));
	connect(commentwindow, SIGNAL(commentChanged(QString&)), this, SLOT(commentChanged(QString&)));

//	boardwindow->setVisible(false);
//	enginewindow->setVisible(false);
//	openingwindow->setVisible(false);
//	movewindow->setVisible(false);
//	commentwindow->setVisible(false);

	restoreState(settings.value("mainWindowState").toByteArray());
}

MainWindow::~MainWindow()
{
	delete currentPath;
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
	importPgnAct = fileMenu->addAction("Import pgnfiles", this, &MainWindow::fileImportPgn);
	fileMenu->addSeparator();
	exitAct = fileMenu->addAction("Exit", this, &QWidget::close);

	bookMenu = menuBar()->addMenu("Book");
	bookWriteMenu = bookMenu->addMenu("Write enable");
	writeTheoryAct = bookWriteMenu->addAction("Write to theory book", this, &MainWindow::bookWriteTheory);
	writeRepAct = bookWriteMenu->addAction("Write to repertoire book", this, &MainWindow::bookWriteRep);
	writeTheoryAct->setCheckable(true);
	writeRepAct->setCheckable(true);
	bookMenu->addSeparator();
	whiteRepAct = bookMenu->addAction("White repertoire", this, &MainWindow::whiteRepertoire);
	blackRepAct = bookMenu->addAction("Black repertoire", this, &MainWindow::blackRepertoire);

	// Setting up the toolbar
	toolbar = addToolBar("Toolbar");
	toolbar->addAction(writeTheoryAct);
	toolbar->addAction(writeRepAct);
	toolbar->addAction(whiteRepAct);
	toolbar->addAction(blackRepAct);

	// No database opened as default
	closeTheoryAct->setDisabled(true);
	closeRepAct->setDisabled(true);
	writeTheoryAct->setDisabled(true);
	writeRepAct->setDisabled(true);
	whiteRepAct->setDisabled(true);
	blackRepAct->setDisabled(true);
	importPgnAct->setDisabled(true);

	whiteRepAct->setCheckable(true);
	blackRepAct->setCheckable(true);
	whiteRepAct->setChecked(true);
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
//	writeSettings();
//	event->accept();
}

void MainWindow::fileOpenTheory()
{
	QMessageBox msgbox;
	QString path = QFileDialog::getOpenFileName(this, "Open book", dataPath, "Book files (*.book)");
	if (!path.isEmpty())
	{
		if (!theoryBase->open(path))
		{
			msgbox.setText("Can't open book");
			msgbox.exec();
			return;
		}
		bdeTheory = theoryBase->find(currentPath->getPosition(),0);
		movewindow->update(bdeTheory, bdeRep);
		openingwindow->update(bdeTheory, bdeRep);
		commentwindow->update(bdeTheory.comment, bdeRep.comment);
/*
		movewindow->setVisible(true);
		commentwindow->setVisible(true);
		*/

		closeTheoryAct->setDisabled(false);
		writeTheoryAct->setDisabled(false);
		importPgnAct->setDisabled(false);

		if (writeTheory)
		{
			commentwindow->setWriteTheory(false);
			writeTheory = false;
			writeTheoryAct->setChecked(false);
		}
	}
}

void MainWindow::fileOpenRep()
{
	QMessageBox msgbox;
	QString path = QFileDialog::getOpenFileName(this, "Open book", dataPath, "Book files (*.book)");
	if (!path.isEmpty())
	{
		if (!repBase->open(path))
		{
			msgbox.setText("Can't open book");
			msgbox.exec();
			return;
		}
		bdeRep = repBase->find(currentPath->getPosition(),whiteRep?1:2);
		movewindow->update(bdeTheory, bdeRep);
		openingwindow->update(bdeTheory, bdeRep);
		commentwindow->update(bdeTheory.comment, bdeRep.comment);
		/*
		movewindow->setVisible(true);
		commentwindow->setVisible(true);
		*/

		closeRepAct->setDisabled(false);
		writeRepAct->setDisabled(false);
		whiteRepAct->setDisabled(false);
		blackRepAct->setDisabled(false);
		importPgnAct->setDisabled(false);

		if (writeRep)
		{
			commentwindow->setWriteRep(false);
			writeRep = false;
			writeRepAct->setChecked(false);
		}
	}
}

void MainWindow::fileNewTheory()
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
		theoryBase->create(path);
		bdeTheory.clear();
		bdeTheory.board = currentPath->getStartPosition();
		bdeTheory.eco = "A00";
		theoryBase->add(bdeTheory);

		bdeTheory = theoryBase->find(currentPath->getPosition(), 0);
		
		movewindow->update(bdeTheory, bdeRep);
		openingwindow->update(bdeTheory, bdeRep);
		commentwindow->update(bdeTheory.comment, bdeRep.comment);
		/*
		movewindow->setVisible(true);
		commentwindow->setVisible(true);
		*/
		closeTheoryAct->setDisabled(false);
		writeTheoryAct->setDisabled(false);
		importPgnAct->setDisabled(false);

		if (writeTheory)
		{
			commentwindow->setWriteTheory(false);
			writeTheory = false;
			writeTheoryAct->setChecked(false);
		}
	}
}

void MainWindow::fileNewRep()
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
		repBase->create(path);
		bdeRep.clear();
		bdeRep.board = currentPath->getStartPosition();
		bdeRep.repertoire = 1;
		repBase->add(bdeRep);
		bdeRep.repertoire = 2;
		repBase->add(bdeRep);

		bdeRep = repBase->find(currentPath->getPosition(), whiteRep ? 1 : 2);

		movewindow->update(bdeTheory, bdeRep);
		openingwindow->update(bdeTheory, bdeRep);
		commentwindow->update(bdeTheory.comment, bdeRep.comment);
		/*
		movewindow->setVisible(true);
		commentwindow->setVisible(true);
		*/

		closeRepAct->setDisabled(false);
		writeRepAct->setDisabled(false);
		whiteRepAct->setDisabled(false);
		blackRepAct->setDisabled(false);
		importPgnAct->setDisabled(false);

		if (writeRep)
		{
			commentwindow->setWriteRep(false);
			writeRep = false;
			writeRepAct->setChecked(false);
		}
	}
}

void MainWindow::fileCloseTheory()
{
	theoryBase->close();
	bdeTheory.clear();
	closeTheoryAct->setDisabled(true);
	writeTheoryAct->setDisabled(true);

	if (writeTheory)
	{
		commentwindow->setWriteTheory(false);
		writeTheory = false;
		writeTheoryAct->setChecked(false);
	}
	if (!repBase->isOpen())
		importPgnAct->setDisabled(true);
}

void MainWindow::fileCloseRep()
{
	repBase->close();
	bdeRep.clear();
	closeRepAct->setDisabled(true);
	writeRepAct->setDisabled(true);
	whiteRepAct->setDisabled(true);
	blackRepAct->setDisabled(true);

	if (writeRep)
	{
		commentwindow->setWriteRep(false);
		writeRep = false;
		writeRepAct->setChecked(false);
	}
	if (!theoryBase->isOpen())
		importPgnAct->setDisabled(true);
}

void MainWindow::bookWriteTheory()
{
	writeTheory = writeTheory ? false : true;
	writeTheoryAct->setChecked(writeTheory);
	writeRep = false;
	writeRepAct->setChecked(false);
	commentwindow->setWriteTheory(writeTheory);
}

void MainWindow::bookWriteRep()
{
	writeRep = writeRep ? false : true;
	writeRepAct->setChecked(writeRep);
	writeTheory = false;
	writeTheoryAct->setChecked(false);
	commentwindow->setWriteRep(writeRep);
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
	ChessBoard board = currentPath->getPosition();
	BookDBMove bm;
	int i;

	// Do the move if it is legal
	if (!currentPath->add(move))
	{
		boardwindow->setPosition(board);
		return;
	}

	// Save the move if it doesn't exist
	if (writeTheory)
	{
		if (!bdeTheory.moveExist(move))
		{
			bm.clear();
			bm.move = move;
			bdeTheory.movelist.append(bm);
			theoryBase->add(bdeTheory);
		}
	} else if (writeRep)
	{
		if (!bdeRep.moveExist(move))
		{
			bm.clear();
			bm.move = move;
			bdeRep.movelist.append(bm);
			repBase->add(bdeRep);
		}
		else
		{
			for (i = 0; i < bdeRep.movelist.size(); i++)
			{
				if (bdeRep.movelist[i].move == move)
				{
					repBase->add(bdeRep);
					break;
				}
			}
		}
	}

	// Change to read from both db
	board = currentPath->getPosition();
	bdeTheory = theoryBase->find(board, 0);
	bdeRep = repBase->find(board, whiteRep ? 1 : 2);
	boardwindow->setPosition(board);
	enginewindow->setPosition(board,currentPath->size()/2+1);
	movewindow->update(bdeTheory, bdeRep);
	openingwindow->update(bdeTheory, bdeRep);
	commentwindow->update(bdeTheory.comment, bdeRep.comment);
	pathwindow->update(currentPath);
}

void MainWindow::pathSelected(int ply)
{
	if (ply < 1)
		currentPath->clear();
	else
		currentPath->setLength(ply);

	ChessBoard board = currentPath->getPosition();
	bdeTheory = theoryBase->find(board, 0);
	bdeRep = repBase->find(board, whiteRep ? 1 : 2);
	boardwindow->setPosition(board);
	enginewindow->setPosition(board, currentPath->size() / 2 + 1);
	movewindow->update(bdeTheory, bdeRep);
	openingwindow->update(bdeTheory, bdeRep);
	commentwindow->update(bdeTheory.comment, bdeRep.comment);
	pathwindow->update(currentPath);
}

void MainWindow::commentChanged(QString& comment)
{
	if (writeTheory)
	{
		bdeTheory.comment = comment;
		theoryBase->add(bdeTheory);
	}
	else if (writeRep)
	{
		bdeRep.comment = comment;
		repBase->add(bdeTheory);
	}
}

void MainWindow::whiteRepertoire()
{
	whiteRep = true;
	blackRepAct->setChecked(false);
	ChessBoard board = currentPath->getPosition();
	bdeRep = repBase->find(board, 1);
	movewindow->update(bdeTheory, bdeRep);
	openingwindow->update(bdeTheory, bdeRep);
	commentwindow->update(bdeTheory.comment, bdeRep.comment);
}

void MainWindow::blackRepertoire()
{
	whiteRep = false;
	whiteRepAct->setChecked(false);
	ChessBoard board = currentPath->getPosition();
	bdeRep = repBase->find(board, 2);
	movewindow->update(bdeTheory, bdeRep);
	openingwindow->update(bdeTheory, bdeRep);
	commentwindow->update(bdeTheory.comment, bdeRep.comment);
}

void MainWindow::fileImportPgn()
{
	int moves=999;
	ImportPgnDialog dialog(this);
	dialog.setItems(theoryBase->isOpen(), repBase->isOpen(), whiteRep, moves);
	if (dialog.exec() == QDialog::Rejected)
		return;
	bool theory, rep, white, variation, comment;
	QString path;
	dialog.getItems(path, theory, rep, white, moves, comment, variation);

	if (theory)
		dialog.importPgnFile(this, theoryBase, path, false, false, moves, comment, variation);
	else if (rep)
		dialog.importPgnFile(this, repBase, path, white, !white, moves, comment, variation);

	// Change to read from both db
	ChessBoard board = currentPath->getPosition();
	bdeTheory = theoryBase->find(board, 0);
	bdeRep = repBase->find(board, whiteRep ? 1 : 2);
	boardwindow->setPosition(board);
	movewindow->update(bdeTheory, bdeRep);
	openingwindow->update(bdeTheory, bdeRep);
	commentwindow->update(bdeTheory.comment, bdeRep.comment);
}