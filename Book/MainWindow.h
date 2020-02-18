#pragma once

#include "Database.h"
#include "Statistics.h"
#include "Computer.h"
#include "Training.h"
#include "Openings.h"
#include "StatusWatch.h"
#include "MoveTableWindow.h"
#include "OpeningWindow.h"
#include "CommentWindow.h"
#include "PathWindow.h"
#include "EngineWindow.h"
#include "TrainingWindow.h"
#include "Path.h"
#include "../Common/ChessMove.h"
#include "../Common/BoardWindow.h"
#include "../Common/DgtBoard.h"
#include "../Common/MoveList.h"
#include <QMainWindow>
#include <QString>
#include <QTranslator>
#include <QActionGroup>
#include <QMenu>
#include <QToolBar>
#include <QAction>
#include <QCloseEvent>
#include <QSplitter>
#include <QToolButton>
#include <QColor>
#include <QVector>

enum {THEORY=0, REPWHITE, REPBLACK };


class MainWindow : public QMainWindow
{
	Q_OBJECT

private:
	void closeEvent(QCloseEvent* event) override;
	void aboutDialog();

protected:
	void changeEvent(QEvent*);

public slots:
	void moveEntered(ChessMove&);
	void moveSelected(ChessMove&);
	void childNeedRefresh();
	void moveDelete(int rep, const ChessMove&);
	void moveSetAsMain(int rep, const ChessMove&);
	void pathSelected(int);
	void pathToDB(int);
	void pathCopy();
	void pathPaste();
	void commentChanged(QString&, int);
	void addMoveComment(int rep, int movenr, QString& comment);
	void enginePV(ComputerDBEngine&, ChessBoard&);
	void slotLanguageChanged(QAction*);
	void dgtStatus(int);
	void dgtStatusClicked(bool);
	void toolbarAction(QAction*);
	void trainingFlipBoard(int color);
	void trainingAddMoves(MoveList&);
	void trainingSetArrow(int fromSq, int toSq, bool wrong, int sec);
#ifdef _DEBUG
	void test();
#endif
public:
	Path* currentPath;
	MainWindow(QWidget *parent = Q_NULLPTR);
	virtual ~MainWindow();

private:
	QVector<StatisticsDBMove> statList;
	QVector<ComputerDBEngine> compList;
	QTranslator translator;
	QString locale;
	QColor goodColor;
	QColor badColor;
	QMenu* fileMenu;
	QMenu* bookMenu;
	QMenu* bookWriteMenu;
	QMenu* trainingMenu;
	QMenu* settingsMenu;
	QMenu* langMenu;
	QToolBar* toolbar;
	QAction* writeAct[3];
	QAction* exitAct;
	QAction* clearTrainingAct;
	QAction* createTrainingAct;
	QAction* startTrainingAct;
	QAction* stopTrainingAct;
	QAction* setupDatabaseAct;
	QAction* useDgtAct;
	QAction* engAct;
	QAction* norAct;
	QActionGroup * langGroup;
	QAction* toolbarTrainingAct;
	QToolButton* dgtIcon;
	QSplitter* hSplitter;
	QSplitter* v1Splitter;
	QSplitter* v2Splitter;
	BoardWindow* boardwindow;
	MoveTableWindow* movewindow;
	CommentWindow* commentwindow;
	OpeningWindow* openingwindow;
	PathWindow* pathwindow;
	EngineWindow* enginewindow;
	TrainingWindow* trainingwindow;
	Database* Base[3];
	Statistics* statisticsDB;
	Computer* computerDB;
	Openings* openingsDB;
	BookDBEntry bde[3];
	StatisticsDBEntry sde;
	ComputerDBEntry cde;
	Training* trainingDB;
	DgtBoard* dgt;
	StatusWatch* statusWatch;
	// You are in training mode;
	bool inTraining;
	// Which DB to write to when moving pieces on the chessboard. (-1=none)
	int write;
	void createMenu();
	void createStatusbar();
	void updateWindow();
	void readDB();
	void updateMenu();
	void writeSettings();
	void readSettings();
	void flipBoard();
	void bookWriteTheory() { bookWrite(THEORY); };
	void bookWriteWhite() { bookWrite(REPWHITE); };
	void bookWriteBlack() { bookWrite(REPBLACK); };
	void bookWrite(int);
	void trainingStop();
	void trainingStart();
	void trainingClearData();
	void trainingCreate();
	//void trainingRun(int color, ChessBoard& pos);
	//void trainingStartBoth();
	//void trainingStartWhite();
	//void trainingStartBlack();
	//void trainingStartPosBoth();
	//void trainingStartPosWhite();
	//void trainingStartPosBlack();

	void setLanguage();
	void loadLanguage();
	void retranslateUi();
	void setupDatabase();
	void useDgt();
};
