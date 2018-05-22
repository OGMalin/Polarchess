#include "MainWindow.h"
#include <QtWidgets>
#include <QIcon>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	createMenu();
	createStatusBar();
}

void MainWindow::createMenu()
{
	QMenu* fileMenu = menuBar()->addMenu(tr("File"));

//	fileMenu->addSeparator();
	QAction *exitAct = fileMenu->addAction(tr("Exit"), this, &QWidget::close);
	exitAct->setStatusTip(tr("Exit the application"));
}

void MainWindow::createStatusBar()
{
	statusBar()->showMessage(tr("Ready"));
}