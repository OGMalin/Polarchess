#pragma once

#include "Database.h"
#include "Statistics.h"
#include "Computer.h"
#include "Training.h"
#include "StatusWatch.h"
#include "../Common/ChessMove.h"
#include <QMainWindow>
#include <QString>
#include <QTranslator>
#include <QActionGroup>

enum {THEORY=0, REPWHITE, REPBLACK };
class QMenu;
class QToolBar;
class QAction;
class QCloseEvent;
class QSplitter;
class BoardWindow;
class MoveWindow;
class OpeningWindow;
class CommentWindow;
class PathWindow;
class EngineWindow;
class Path;

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
	void moveSelected(int rep, int movenr);
	void moveDelete(int rep, int movenr);
	void pathSelected(int);
	void pathToDB(int);
	void commentChanged(QString&);
	void addMoveComment(int rep, int movenr, QString& comment);
	void enginePV(ComputerDBEngine&, ChessBoard&);
	void slotLanguageChanged(QAction*);

public:
	Path* currentPath;
	MainWindow(QWidget *parent = Q_NULLPTR);
	virtual ~MainWindow();

private:
	QVector<StatisticsDBMove> statList;
	QVector<ComputerDBEngine> compList;
	QTranslator translator;
	QString dataPath;
	QString dataTheory;
	QString dataWhite;
	QString dataBlack;
	QString dataStatistics;
	QString dataComputer;
	QString locale;
	QMenu* fileMenu;
	QMenu* fileOpenMenu;
	QMenu* fileNewMenu;
	QMenu* fileCloseMenu;
	QMenu* fileImportMenu;
	QMenu* bookMenu;
	QMenu* bookWriteMenu;
	QMenu* trainingMenu;
	QMenu* trainingStartMenu;
	QMenu* settingsMenu;
	QMenu* langMenu;
	QToolBar* toolbar;
	QAction* openAct[3];
	QAction* newAct[3];
	QAction* closeAct[3];
	QAction* writeAct[3];
	QAction* exitAct;
	QAction* importPgnAct;
	QAction* importBookAct;
	QAction* createStatAct;
	QAction* clearTrainingAct;
	QAction* createTrainingAct;
	QAction* startTrainingBothAct;
	QAction* startTrainingWhiteAct;
	QAction* startTrainingBlackAct;
	QAction* startTrainingPosBothAct;
	QAction* startTrainingPosWhiteAct;
	QAction* startTrainingPosBlackAct;
	QAction* stopTrainingAct;
	QAction* engAct;
	QAction* norAct;
	QActionGroup * langGroup;
	QSplitter* hSplitter;
	QSplitter* v1Splitter;
	QSplitter* v2Splitter;
	BoardWindow* boardwindow;
	MoveWindow* movewindow;
	CommentWindow* commentwindow;
	OpeningWindow* openingwindow;
	PathWindow* pathwindow;
	EngineWindow* enginewindow;
	Database* Base[3];
	Statistics* statistics;
	Computer* computer;
	BookDBEntry bde[3];
	StatisticsDBEntry sde;
	ComputerDBEntry cde;
	Training* training;
	TrainingPath trainingLine;
	TrainingStat trainingStat;
	ChessBoard trainingBoard;
	int trainingColor;
	StatusWatch* statusWatch;
	bool inTraining;
	int write;
	void createMenu();
	void createStatusbar();
	void updateWindow();
	void readDB();
	void updateMenu();
	void writeSettings();
	void readSettings();
	void flipBoard();
	void fileOpenTheory() { fileOpen(THEORY); };
	void fileOpenWhite() { fileOpen(REPWHITE); };
	void fileOpenBlack() { fileOpen(REPBLACK); };
	void fileOpen(int);
	void fileNewTheory() { fileNew(THEORY); };
	void fileNewWhite() { fileNew(REPWHITE); };
	void fileNewBlack() { fileNew(REPBLACK); };
	void fileNew(int);
	void fileCloseTheory() { fileClose(THEORY); };
	void fileCloseWhite() { fileClose(REPWHITE); };
	void fileCloseBlack() { fileClose(REPBLACK); };
	void fileClose(int);
	void fileImportPgn();
	void fileImportBook();
	void fileCreateStatistics();
	void bookWriteTheory() { bookWrite(THEORY); };
	void bookWriteWhite() { bookWrite(REPWHITE); };
	void bookWriteBlack() { bookWrite(REPBLACK); };
	void bookWrite(int);
	void trainingClearData();
	void trainingCreate();
	void trainingStart(int color, ChessBoard& pos);
	void trainingStartBoth();
	void trainingStartWhite();
	void trainingStartBlack();
	void trainingStartPosBoth();
	void trainingStartPosWhite();
	void trainingStartPosBlack();
	void trainingStop();
	void setLanguage();
	void loadLanguage();
	void retranslateUi();
};
