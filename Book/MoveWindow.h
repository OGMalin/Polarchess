#pragma once

#include "Database.h"
#include <QWidget>
#include <QBrush>
#include <QFont>
#include <QVector>
#include "Statistics.h"
#include "Computer.h"
#include "../Common/ChessMove.h"

class QTableView;
class QStandardItemModel;

struct MoveTableHeader
{
	QString label;
	bool inUse;
	int column;
	MoveTableHeader() { clear(); };
	void clear() { label.clear(); inUse = false; column = -1; };
};

struct MoveTableEntry
{
	ChessMove move;
	int whitewin, draw, blackwin, elo, year;
	QString engine;
	int cp;
	int depth;
	int rep[3];
	void clear() { move.clear(); whitewin = draw = blackwin = elo = year = 0; engine.clear(); cp = depth = 0; rep[0] = rep[1] = rep[2] = 0; };
};

class MoveWindow : public QWidget
{
	Q_OBJECT

public:
	Computer* computer;
	MoveWindow(QWidget *parent=0);
	~MoveWindow();
	void refresh(BookDBEntry& theory, BookDBEntry& white, BookDBEntry& black, StatisticsDBEntry& statistics, ComputerDBEntry& compdata, ChessBoard&, int movenr);
	void addComment(QString& comment);

signals:
	void moveSelected(int rep, int movenr);
	void moveDelete(int rep, int movenr);
	void addMoveComment(int rep, int movenr, QString& comment);

public slots:
	void showContextMenu(const QPoint& pos);
	void selectFont();
	void deleteMove();
	void moveClicked(const QModelIndex& index);
	void addComment0() { addComment(QString("")); };
	void addComment1() { addComment(QString("!")); };
	void addComment2() { addComment(QString("?")); };
	void addComment3() { addComment(QString("!!")); };
	void addComment4() { addComment(QString("??")); };
	void addComment5() { addComment(QString("!?")); };
	void addComment6() { addComment(QString("?!")); };

private:
	QTableView * table;
	QStandardItemModel* model;
	QBrush repBrush;
	QBrush normalBrush;
	QFont font;
	ChessBoard currentBoard;
	int currentMoveNr;
	MoveTableHeader hMoves;
	MoveTableHeader hTheory;
	MoveTableHeader hWhite;
	MoveTableHeader hBlack;
	MoveTableHeader hScore;
	MoveTableHeader hGames;
	MoveTableHeader hComp;
	MoveTableHeader hWin;
	MoveTableHeader hDraw;
	QVector<MoveTableEntry> movetable;
	void add(BookDBEntry&, int rep, ChessBoard&);
	void add(StatisticsDBEntry&);
	void add(ComputerDBEntry&);
	int existInTable(ChessMove& m);
	void refresh();
};
