#pragma once

#include "Database.h"
#include <QWidget>
#include <QBrush>
#include <QFont>
#include <QVector>
#include <QTableView>
#include <QStandardItemModel>
#include "Statistics.h"
#include "Computer.h"
#include "../Common/ChessMove.h"
#include "../Common/PolyglotBook.h"

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
	int whitewin, draw, blackwin, elo, year, polyglot;
	QString engine;
	int cp;
	int rep[3];
	void clear() { move.clear(); whitewin = draw = blackwin = elo = year = polyglot = 0; engine.clear(); cp = 0; rep[0] = rep[1] = rep[2] = 0; };
};

class MoveTableWindow : public QWidget
{
	Q_OBJECT

public:
	Computer* computerDB;
	MoveTableWindow(QWidget *parent=0);
	~MoveTableWindow();
	void refresh(BookDBEntry& theory, BookDBEntry& white, BookDBEntry& black, StatisticsDBEntry& statistics, ComputerDBEntry& compdata, ChessBoard&, int movenr, PolyglotBook*);
	void addComment(QString& comment);
	QString fontToString();
	void fontFromString(const QString&);

signals:
	void moveSelected(ChessMove&);
	void moveDelete(int rep, const ChessMove&);
	void moveSetAsMain(int rep, const ChessMove&);
	void addMoveComment(int rep, int movenr, QString& comment);
	void needRefresh();

public slots:
	void showContextMenu(const QPoint& pos);
	void selectFont();
	void deleteMove(int rep, const ChessMove&);
	void setMainMove(int rep, const ChessMove&);
	void rearrangeEngines();
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
	ChessBoard currentBoard;
	int currentMoveNr;
	MoveTableHeader hMoves;
	MoveTableHeader hTheory;
	MoveTableHeader hWhite;
	MoveTableHeader hBlack;
	MoveTableHeader hWDL;
	MoveTableHeader hGames;
	MoveTableHeader hComp;
	MoveTableHeader hScore;
	MoveTableHeader hDraw;
	MoveTableHeader hElo;
	MoveTableHeader hYear;
	MoveTableHeader hPolyglot;
	QVector<MoveTableEntry> movetable;
	void add(BookDBEntry&, int rep, ChessBoard&);
	void add(StatisticsDBEntry&);
	void add(ComputerDBEntry&);
	void add(PolyglotBook*, ChessBoard&);
	int existInTable(ChessMove& m);
	void refresh();
};
