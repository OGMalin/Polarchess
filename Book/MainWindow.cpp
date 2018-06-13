#include "MainWindow.h"
#include "MoveWindow.h"
#include "OpeningWindow.h"
#include "CommentWindow.h"
#include "AboutDialog.h"
#include "../Common/BoardWindow.h"
#include <QMenu>
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

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	readonly = true;
	createMenu();
	readSettings();

	statusBar();

	hSplitter = new QSplitter(Qt::Horizontal);
	vSplitter = new QSplitter(Qt::Vertical);

	boardwindow = new BoardWindow;
	openingwindow = new OpeningWindow;
	movewindow = new MoveWindow;
	commentwindow = new CommentWindow;

	hSplitter->addWidget(boardwindow);
	hSplitter->addWidget(vSplitter);
	vSplitter->addWidget(movewindow);
	vSplitter->addWidget(openingwindow);
	vSplitter->addWidget(commentwindow);

	setCentralWidget(hSplitter);

	database = new Database();
	connect(boardwindow, SIGNAL(moveEntered(ChessMove&)), this, SLOT(moveEntered(ChessMove&)));
}

void MainWindow::createMenu()
{
	// File menu
	fileMenu = menuBar()->addMenu("File");
	openAct = fileMenu->addAction("Open book", this, &MainWindow::fileOpen);
	newAct = fileMenu->addAction("New book", this, &MainWindow::fileNew);
	fileMenu->addSeparator();
	exitAct = fileMenu->addAction("Exit", this, &QWidget::close);

	bookMenu = menuBar()->addMenu("Book");
	readonlyAct = bookMenu->addAction("Readonly", this, &MainWindow::bookReadonly);
	readonlyAct->setCheckable(true);
	if (readonly)
		readonlyAct->setChecked(true);
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

void MainWindow::fileOpen()
{
	QString path = QStandardPaths::locate(QStandardPaths::DocumentsLocation, "/Polarchess", QStandardPaths::LocateDirectory);
	path = QFileDialog::getOpenFileName(this, "Open book", path, "Book files (*.book)");
	if (!path.isEmpty())
	{
		if (!database->open(path))
		{
			QMessageBox msgbox;
			msgbox.setText("Can't open book");
			msgbox.exec();
			return;
		}
		current.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPPRNBQKBNR w KQkq - 0 1");
		bde = database->find(QString(current.getFen(true).c_str()));
		boardwindow->setPosition(bde.fen);
		movewindow->update(bde);
		openingwindow->update(bde);
		commentwindow->update(bde);
	}
}

void MainWindow::fileNew()
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
		database->create(path);
		current.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPPRNBQKBNR w KQkq - 0 1");
		bde.clear();
		bde.fen=current.getFen(true).c_str();
		bde.eco = "A00";
		database->add(bde);
		boardwindow->setPosition(bde.fen);
	}
}

void MainWindow::bookReadonly()
{
	readonly = readonly ? false : true;
	readonlyAct->setChecked(readonly);
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
}
