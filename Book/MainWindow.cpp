#include "MainWindow.h"
#include <QMenu>
#include <QAction>
#include <QMenuBar>
#include <QSettings>
#include <QApplication>
#include <QRect>
#include <QDesktopWidget>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	createMenu();
	readSettings();
}

void MainWindow::createMenu()
{
	// File menu
	fileMenu = menuBar()->addMenu("File");
	exitAct = fileMenu->addAction("Exit", this, &QWidget::close);
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


