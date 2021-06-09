#pragma once

#include <QWidget>
#include <QVector>
#include <QTableView>
#include <QStandardItemModel>
#include "../Common/ChessMove.h"
#include "../Common/ChessBoard.h"
#include "../Common/ChessBoard.h"
#include "../Common/PolyglotBook.h"

struct MoveTableEntry
{
	ChessMove move;
	int weight,learn;
	void clear() { move.clear(); weight = learn = 0; };
};

class MoveTableWindow : public QWidget
{
	Q_OBJECT

public:
	MoveTableWindow(QWidget *parent=0);
	~MoveTableWindow();
	void refresh(ChessBoard&, PolyglotBook&);
//	void refresh(BookDBEntry& theory, BookDBEntry& white, BookDBEntry& black, StatisticsDBEntry& statistics, ComputerDBEntry& compdata, ChessBoard&, int movenr);

signals:
	//void moveSelected(ChessMove&);
	//void moveDelete(int rep, const ChessMove&);
	//void moveSetAsMain(int rep, const ChessMove&);
	//void addMoveComment(int rep, int movenr, QString& comment);
	//void needRefresh();

public slots:
	void showContextMenu(const QPoint& pos);
	//void selectFont();
	//void deleteMove(int rep, const ChessMove&);
	//void setMainMove(int rep, const ChessMove&);
	//void rearrangeEngines();
	//void moveClicked(const QModelIndex& index);
	//void addComment0() { addComment(QString("")); };
	//void addComment1() { addComment(QString("!")); };
	//void addComment2() { addComment(QString("?")); };
	//void addComment3() { addComment(QString("!!")); };
	//void addComment4() { addComment(QString("??")); };
	//void addComment5() { addComment(QString("!?")); };
	//void addComment6() { addComment(QString("?!")); };

private:
	QTableView * table;
	QStandardItemModel* model;
	QBrush repBrush;
	QBrush normalBrush;
	ChessBoard currentBoard;
	int currentMoveNr;
	QVector<MoveTableEntry> movetable;
	//void add(BookDBEntry&, int rep, ChessBoard&);
	//void add(StatisticsDBEntry&);
	//void add(ComputerDBEntry&);
	int existInTable(ChessMove& m);
	void refresh();
};
