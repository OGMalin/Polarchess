#pragma once

#include "Database.h"
#include "Training.h"
#include "StatusWatch.h"
#include "../Common/ChessMove.h"
#include <QMainWindow>
#include <QString>

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

public slots:
	void moveEntered(ChessMove&);
	void moveSelected(int rep, int movenr);
	void moveDelete(int rep, int movenr);
	void pathSelected(int);
	void pathToDB(int);
	void commentChanged(QString&);
	void addMoveComment(int rep, int movenr, QString& comment);

public:
	Path* currentPath;
	MainWindow(QWidget *parent = Q_NULLPTR);
	virtual ~MainWindow();

private:
	QString dataPath;
	QString dataTheory;
	QString dataWhite;
	QString dataBlack;
	QMenu* fileMenu;
	QMenu* fileOpenMenu;
	QMenu* fileNewMenu;
	QMenu* fileCloseMenu;
	QMenu* fileImportMenu;
	QMenu* bookMenu;
	QMenu* bookWriteMenu;
	QMenu* trainingMenu;
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
	QAction* createTrainingFullAct;
	QAction* createTrainingPosAct;
	QAction* startTrainingAct;
	QAction* stopTrainingAct;
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
	BookDBEntry bde[3];
	Training* training;
	TrainingPath trainingLine;
	StatusWatch* statusWatch;
	bool inTraining;
	int write;
	void createMenu();
	void createStatusbar();
	void updateWindow();
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
	void trainingCreateFull();
	void trainingCreateFromPos();
	void trainingStart();
	void trainingStop();
};
