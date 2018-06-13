#pragma once

#include "Database.h"
#include "../Common/ChessMove.h"
#include "../Common/ChessBoard.h"
#include <QMainWindow>

class QMenu;
class QAction;
class QCloseEvent;
class QSplitter;
class BoardWindow;
class MoveWindow;
class OpeningWindow;
class CommentWindow;


class MainWindow : public QMainWindow
{
	Q_OBJECT

private:
	void closeEvent(QCloseEvent* event) override;
	void aboutDialog();

public slots:
	void moveEntered(ChessMove&);

public:
	MainWindow(QWidget *parent = Q_NULLPTR);

private:
	QMenu* fileMenu;
	QMenu* bookMenu;
	QAction* openAct;
	QAction* newAct;
	QAction* exitAct;
	QAction* readonlyAct;
	QSplitter* hSplitter;
	QSplitter* vSplitter;
	BoardWindow* boardwindow;
	MoveWindow* movewindow;
	OpeningWindow* openingwindow;
	CommentWindow* commentwindow;
	Database* database;
	ChessBoard current;
	BookDBEntry bde;
	bool readonly;
	void createMenu();
	void writeSettings();
	void readSettings();
	void flipBoard();
	void fileOpen();
	void fileNew();
	void bookReadonly();
};
