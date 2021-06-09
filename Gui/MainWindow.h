#pragma once

#include <QMainWindow>
#include <QString>
#include <QTranslator>
#include <QVector>
#include "NewGameDialog.h"
#include "Player.h"
#include "EnginePlayer.h"
#include "../Common/ChessMove.h"

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
class Database;
class QChessGame;

class MainWindow : public QMainWindow
{
	Q_OBJECT

private:
	void closeEvent(QCloseEvent* event) override;
	void aboutDialog();

protected:
	void changeEvent(QEvent*);

signals:
	void clearGame(QString& fen);

public slots:
	void slotLanguageChanged(QAction* action);
//	void playEngineMessage(const QString&);
	void playEngineStarted();
//	void playEngineMove(const QString&, const QString&);
	void clockAlarm(int);
	void moveEntered(ChessMove&);

public:
	QChessGame* currentGame;
	MainWindow();
	virtual ~MainWindow();
	void firstTime();
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
	QAction* resignAct;
	QAction* abortAct;
	QAction* flipAct;
	QAction* installEngineAct;
	QTranslator translator;
	QActionGroup * langGroup;
	QString locale;
	QSplitter* hSplitter;
	QSplitter* vSplitter;
	BoardWindow* boardwindow;
	Scoresheet* scoresheet;
	ClockWindow* clockwindow;
	EngineWindow* enginewindow;
	Engine* engine;
	//Engine* analysisEngine;
	Database* database;
	NewGameSetting gameSetting;
	Player player;
	EnginePlayers engines;
//	EngineFeature engineFeature;
	int engineColor;
	int enginestate; // unloaded=0, installing=1, checking=2, playing=3
	bool running;
	QString installedEngine;
	void createMenu();
	void setLanguage();
	void loadLanguage();
	void retranslateUi();
	void readSettings();
	void writeSettings();
	void setDefaultSettings();
	void flipBoard();
	void newGame();
	void resign();
	void abort();
	void endGame();
	void saveGame();
	void installEngine();
};
