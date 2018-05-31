#pragma once

#include <QMainWindow>
#include <QString>
#include <QTranslator>
#include "../Common/ChessGame.h"
#include "NewGameDialog.h"

class QMenu;
class QToolBar;
class QAction;
class QActionGroup;
class QSplitter;
class BoardWindow;
class Scoresheet;
class ClockWindow;
class EngineWindow;
class Engine;

class MainWindow : public QMainWindow
{
	Q_OBJECT

private:
	void closeEvent(QCloseEvent* event) override;
	void aboutDialog();

protected:
	void changeEvent(QEvent*);

public slots:
	void slotLanguageChanged(QAction* action);
	void slotEngineMessage(const QString&);
public:
	ChessGame currentGame;
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
	NewGameSetting gameSetting;
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
