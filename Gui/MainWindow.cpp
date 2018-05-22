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

	fileMenu->addSeparator();
	const QIcon exitIcon = QIcon::fromTheme("application-exit");
	QAction *exitAct = fileMenu->addAction(exitIcon, tr("Exit"), this, &QWidget::close);
	exitAct->setShortcuts(QKeySequence::Quit);
	exitAct->setStatusTip(tr("Exit the application"));
}

void MainWindow::createStatusBar()
{
	statusBar()->showMessage(tr("Ready"));
}