#pragma once

#include "Database.h"
#include "../Common/ChessMove.h"
#include <QMainWindow>
#include <QString>

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
	void pathSelected(int);
	void commentChanged(QString&);

public:
	Path* currentPath;
	MainWindow(QWidget *parent = Q_NULLPTR);
	virtual ~MainWindow();

private:
	QString dataPath;
	QMenu* fileMenu;
	QMenu* fileOpenMenu;
	QMenu* fileNewMenu;
	QMenu* fileCloseMenu;
	QMenu* bookMenu;
	QMenu* bookWriteMenu;
	QToolBar* toolbar;
	QAction* openTheoryAct;
	QAction* openRepAct;
	QAction* newTheoryAct;
	QAction* newRepAct;
	QAction* closeTheoryAct;
	QAction* closeRepAct;
	QAction* exitAct;
	QAction* writeTheoryAct;
	QAction* writeRepAct;
	QSplitter* hSplitter;
	QSplitter* v1Splitter;
	QSplitter* v2Splitter;
	BoardWindow* boardwindow;
	MoveWindow* movewindow;
	CommentWindow* commentwindow;
	OpeningWindow* openingwindow;
	PathWindow* pathwindow;
	EngineWindow* enginewindow;
	Database* theoryBase;
	Database* repBase;
	BookDBEntry bdeTheory;
	BookDBEntry bdeRep;
	bool writeTheory;
	bool writeRep;
	void createMenu();
	void writeSettings();
	void readSettings();
	void flipBoard();
	void fileOpenTheory();
	void fileOpenRep();
	void fileNewTheory();
	void fileNewRep();
	void fileCloseTheory();
	void fileCloseRep();
	void bookWriteTheory();
	void bookWriteRep();
};
