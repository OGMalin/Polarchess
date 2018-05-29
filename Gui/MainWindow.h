#pragma once

#include <QMainWindow>
#include <QtWidgets>
#include <QString>
#include "BoardWindow.h"
#include "Scoresheet.h"
#include "ClockWindow.h"
#include "EngineWindow.h"
#include "Engine.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

private:
	void closeEvent(QCloseEvent* event) override;
protected:
	void changeEvent(QEvent*);

public slots:
	void slotLanguageChanged(QAction* action);
	void slotEngineMessage(const QString&);
public:
	MainWindow();
	virtual ~MainWindow();
private:
	QMenu * fileMenu;
	QMenu* settingsMenu;
	QMenu* helpMenu;
	QMenu* langMenu;
	QMenu* boardMenu;
	QMenu* gameMenu;
	QToolBar* toolbar;
	QAction* exitAct;
	QAction* engAct;
	QAction* norAct;
	QAction* defAct;
	QAction* aboutAct;
	QAction* newGameAct;
	QAction* flipAct;
	QTranslator translator;
	QActionGroup * langGroup;
	QString locale;
	QSplitter* hSplitter;
	QSplitter* vSplitter;
	BoardWindow* boardwindow;
	Scoresheet* scoresheet;
	ClockWindow* clockwindow;
	EngineWindow* enginewindow;
	Engine* playEngine;
	Engine* analysisEngine;
	void createMenu();
	void setLanguage();
	void loadLanguage();
	void retranslateUi();
	void readSettings();
	void writeSettings();
	void setDefaultSettings();
	void flipBoard();
	void newGame();
};
