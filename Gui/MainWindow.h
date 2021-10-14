#pragma once

#include "NewGameDialog.h"
#include "Player.h"
#include "EnginePlayer.h"
#include "ClockWindow.h"
#include "EngineWindow.h"
#include "Database.h"
#include "Scoresheet.h"
#include "Engine.h"
#include "Sound.h"
#include "Book.h"
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
	bool gameFinnish();
	bool canWin(typeColor c); // Checked when someone run out of time 

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
	void dgtNewMove(ChessMove*);
	void dgtNewGame();
	void dgtResult(int);
	void dgtStatusClicked(bool);
	void copy();
	void moveSelected(int);

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
	QAction* copyAct;
	QAction* flipAct;
	QAction* installEngineAct;
	QAction* useDgtAct;
	QAction* saveToPgnAct;
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
	Sound* sound;
	Book* book;
//	DgtDLL* dgt;
	DgtBoard* dgt;
	DgtSetting dgtSetting;
	//Engine* analysisEngine;
	Database* database;
	NewGameSetting gameSetting;
	Player player;
	EnginePlayers engines;
	//	EngineFeature engineFeature;
	int engineColor;
	int enginestate; // unloaded=0, installing=1, checking=2, playing=3
	bool running;
	bool autoSaveToPgn;
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
	void setSound();
	void saveToPgn();
	void doEngineMove();
};
