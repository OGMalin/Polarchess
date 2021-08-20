#pragma once

#include "NewGameDialog.h"
#include "Player.h"
#include "EnginePlayer.h"
#include "ClockWindow.h"
#include "EngineWindow.h"
#include "Database.h"
#include "Scoresheet.h"
#include "Engine.h"
#include "SoundDialog.h"
#include "../Common/BoardWindow.h"
#include "../Common/QChessGame.h"
#include "../Common/ChessMove.h"
#include "../Common/DgtBoard.h"
//#include "../Common/DgtDLL.h"
#include <QMainWindow>
#include <QString>
#include <QTranslator>
#include <QVector>
#include <QToolButton>
#include <QSplitter>
#include <QToolBar>
#include <QMenu>
#include <QAction>
#include <QToolBar>
#include <QActionGroup>
#include <QSplitter>

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
	void playEngineMove(const QString&, const QString&);
	void clockAlarm(int);
	void moveEntered(ChessMove&);
	void dgtStatus(int);
	void dgtNewMove(ChessMove&);
	void dgtNewGame();
	void dgtResult(int);
	void dgtStatusClicked(bool);

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
	QAction* soundAct;
	QAction* newGameAct;
	QAction* resignAct;
	QAction* abortAct;
	QAction* flipAct;
	QAction* installEngineAct;
	QAction* useDgtAct;
	QToolButton* dgtIcon;
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
//	DgtDLL* dgt;
	DgtBoard* dgt;
	//Engine* analysisEngine;
	Database* database;
	NewGameSetting gameSetting;
	Player player;
	EnginePlayers engines;
	SoundSetting soundSetting;
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
	void useDgt();
	void sound();
};
