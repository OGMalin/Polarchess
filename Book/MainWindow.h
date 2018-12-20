#pragma once

#include "Database.h"
#include "../Common/ChessMove.h"
#include <QMainWindow>

class QMenu;
class QToolBar;
class QAction;
class QCloseEvent;
class QSplitter;
class BoardWindow;
class MoveWindow;
class OpeningWindow;
class CommentWindow;
class QChessGame;


class MainWindow : public QMainWindow
{
	Q_OBJECT

private:
	void closeEvent(QCloseEvent* event) override;
	void aboutDialog();

public slots:
	void moveEntered(ChessMove&);

public:
	QChessGame* currentGame;
	MainWindow(QWidget *parent = Q_NULLPTR);
	virtual ~MainWindow();

private:
	QMenu* fileMenu;
	QMenu* bookMenu;
	QToolBar* toolbar;
	QAction* openAct;
	QAction* newAct;
	QAction* exitAct;
	QAction* readwriteAct;
	QSplitter* hSplitter;
	QSplitter* vSplitter;
	BoardWindow* boardwindow;
	MoveWindow* movewindow;
	CommentWindow* commentwindow;
	OpeningWindow* openingwindow;
	Database* database;
	BookDBEntry bde;
	bool readwrite;
	void createMenu();
	void writeSettings();
	void readSettings();
	void flipBoard();
	void fileOpen();
	void fileNew();
	void bookReadWrite();
};
