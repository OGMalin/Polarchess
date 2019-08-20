#pragma once

#include "Database.h"
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
	QAction* openAct[3];
	QAction* newAct[3];
	QAction* closeAct[3];
	QAction* writeAct[3];
	QAction* exitAct;
	QAction* importPgnAct;
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
	int write;
	void createMenu();
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
	void bookWriteTheory() { bookWrite(THEORY); };
	void bookWriteWhite() { bookWrite(REPWHITE); };
	void bookWriteBlack() { bookWrite(REPBLACK); };
	void bookWrite(int);
	void fileImportPgn();
};
