#pragma once

#include <QMainWindow>

class QMenu;
class QAction;
class QCloseEvent;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = Q_NULLPTR);

private:
	QMenu * fileMenu;
	QAction* exitAct;

	void closeEvent(QCloseEvent* event) override;
	void createMenu();
	void writeSettings();
	void readSettings();
};
