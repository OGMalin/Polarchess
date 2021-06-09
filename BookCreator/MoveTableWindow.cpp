#include "MoveTableWindow.h"
//#include "ListOrderDialog.h"
//#include "WDLBar.h"
#include <QVBoxLayout>
#include <QStringList>
#include <QVector>
#include <QMenu>
#include <QFontDialog>
#include <QTextStream>
#include <QHeaderView>

MoveTableWindow::MoveTableWindow(QWidget *parent)
	: QWidget(parent)
{
	QVBoxLayout* vbox = new QVBoxLayout;
	model = new QStandardItemModel(0, 3);
	table = new QTableView;
	table->setModel(model);
	model->setHorizontalHeaderItem(0, new QStandardItem("Move"));
	model->setHorizontalHeaderItem(1, new QStandardItem("Weight"));
	model->setHorizontalHeaderItem(2, new QStandardItem("Learn"));
	table->setSelectionMode(QAbstractItemView::NoSelection);
	table->setEditTriggers(QAbstractItemView::NoEditTriggers);
	vbox->setMargin(0);
	vbox->addWidget(table);
	setLayout(vbox);
//	setContentsMargins(0, 0, 0, 0);

	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));
//	connect(table, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(moveClicked(const QModelIndex&)));

//	refresh();
}

MoveTableWindow::~MoveTableWindow()
{
}

//void MoveTableWindow::add(BookDBEntry& book, int rep, ChessBoard& cb)
//{
//	MoveTableEntry mte;
//	QVector<BookDBMove>::iterator bit;
//	int index,val;
//
//	bit = book.movelist.begin();
//	while (bit != book.movelist.end())
//	{
//		if (rep == 0)
//		{
//			val = 1;
//		}
//		else
//		{
//			if (cb.toMove == (rep - 1))
//			{
//				if (bit == book.movelist.begin())
//					val = 2;
//				else
//					val = 3;
//			}
//			else
//			{
//				val = 1;
//			}
//		}
//		index = existInTable(bit->move);
//
//		if (index<0)
//		{
//			mte.clear();
//			mte.move = bit->move;
//			mte.rep[rep] = val;
//			movetable.push_back(mte);
//		}
//		else
//		{
//			movetable[index].rep[rep] = val;
//		}
//		++bit;
//	}
//}
//
//void MoveTableWindow::add(StatisticsDBEntry& stat)
//{
//	MoveTableEntry mte;
//	QVector<StatisticsDBMove>::iterator sit;
//	std::sort(stat.movelist.begin(), stat.movelist.end());
//	sit = stat.movelist.begin();
//	while (sit != stat.movelist.end())
//	{
//		mte.clear();
//		mte.move = sit->move;
//		mte.whitewin = sit->whitewin;
//		mte.draw = sit->draw;
//		mte.blackwin = sit->blackwin;
//		mte.elo = sit->elo;
//		mte.year = sit->year;
//		movetable.push_back(mte);
//		++sit;
//	}
//}
//
//void MoveTableWindow::add(ComputerDBEntry& comp)
//{
//	MoveTableEntry mte;
//	int index;
//	QVector<ComputerDBEngine>::iterator cit;
//	QStringList::iterator eit;
//	eit = computerDB->enginelist.begin();
//	while (eit != computerDB->enginelist.end())
//	{
//		cit = comp.enginelist.begin();
//		while (cit != comp.enginelist.end())
//		{
//			if (*eit == cit->engine)
//			{
//				if (cit->pv.size())
//				{
//					index = existInTable(cit->pv[0]);
//					if (index < 0)
//					{
//						mte.clear();
//						mte.move = cit->pv[0];
//						mte.cp = cit->cp;
//						QTextStream(&mte.engine) << cit->engine << endl << "Cp: " << float(cit->cp/100.0) << ", Depth: " << cit->depth << endl;
//						movetable.push_back(mte);
//					}
//					else if (movetable[index].engine.isEmpty())
//					{
//						movetable[index].cp = cit->cp;
//						QTextStream(&movetable[index].engine) << cit->engine << endl << "Cp: " << float(cit->cp / 100.0) << ", Depth: " << cit->depth << endl;
//					}
//					else
//					{
//						QTextStream(&movetable[index].engine) << cit->engine << endl << "Cp: " << float(cit->cp / 100.0) << ", Depth: " << cit->depth << endl;
//					}
//				}
//			}
//			++cit;
//		}
//		++eit;
//	}
//}
//

int MoveTableWindow::existInTable(ChessMove& m)
{
	int i;
	for (i = 0; i < movetable.size(); i++)
		if (movetable[i].move == m)
			return i;
	return -1;
}

void MoveTableWindow::refresh(ChessBoard& cb, PolyglotBook& b)
{
	ChessMove m;
	char sz[16];
	QStandardItem* item;
	QVector<PolyglotDataEntry> moves;
	b.get(cb, moves);
//	movetable.clear();
	model->setRowCount(0);
	for (int i = 0; i < moves.size(); i++)
	{
		m = b.move(cb, moves[i].move);
		item = new QStandardItem(cb.makeMoveText(m,FIDE).c_str());
		item->setTextAlignment(Qt::AlignCenter);
		model->setItem(i, 0, item);

		item = new QStandardItem(itoa(moves[i].weight, sz, 10));
		item->setTextAlignment(Qt::AlignCenter);
		model->setItem(i, 1, item);

		item = new QStandardItem(itoa(moves[i].learn, sz, 10));
		item->setTextAlignment(Qt::AlignCenter);
		model->setItem(i, 2, item);
	}
	table->verticalHeader()->hide();

//	refresh();
}

//void MoveTableWindow::refresh(BookDBEntry& theory, BookDBEntry& white, BookDBEntry& black, StatisticsDBEntry& stat, ComputerDBEntry& compdata, ChessBoard& cb, int movenr)
//{
//	currentMoveNr = movenr;
//	currentBoard = cb;
//	movetable.clear();
//	add(stat);
//	if (computerDB)
//		add(compdata);
//	add(theory, 0, cb);
//	add(white, 1, cb);
//	add(black, 2, cb);
//	refresh();
//}
//
void MoveTableWindow::refresh()
{
	int i, j;
	int col;
	QString qs;
	QStandardItem* item;

	model->setRowCount(0);
	col = 0;
	for (i = 0; i < movetable.size(); i++)
	{
		// Moves
		if (i == 0)
			model->setHorizontalHeaderItem(0, new QStandardItem("Moves"));
		qs.clear();
		QTextStream(&qs) << currentMoveNr << ((currentBoard.toMove == WHITE) ? "." : "...");
		qs += QString(currentBoard.makeMoveText(movetable[i].move, FIDE).c_str());
		item = new QStandardItem(qs);
		model->setItem(i, 0, item);

		// Weight
		if (i == 0)
			model->setHorizontalHeaderItem(1, new QStandardItem("Weight"));
		if (movetable[i].weight> 0)
		{
			qs.clear();
			QTextStream(&qs) << movetable[i].weight;
			item = new QStandardItem(qs);
			item->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
			model->setItem(i, 1, item);
		}

		// Learn
		if (i == 0)
			model->setHorizontalHeaderItem(2, new QStandardItem("Learn"));
		if (movetable[i].learn > 0)
		{
			qs.clear();
			QTextStream(&qs) << movetable[i].learn;
			item = new QStandardItem(qs);
			item->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
			model->setItem(i, 2, item);
		}
	}

	table->verticalHeader()->hide();
	model->setColumnCount(3);
	model->setRowCount(movetable.size()+1); // +1 pga bug
	table->resizeColumnsToContents();
}

void MoveTableWindow::showContextMenu(const QPoint& pos)
{
//	QMenu* contextMenu = new QMenu(this);
//	QPoint lpos=mapFrom(this, pos);
//	//QModelIndex index = table->indexAt(lpos);
//	//int col = index.column();
//	//int row = index.row();
//	int col = table->columnAt(lpos.x());
//	int row = table->rowAt(lpos.y());
//
//	// Header
//	if (row == 0)
//	{
////		contextMenu->addAction(QString("v"), this, SLOT(addComment0()));
//	}
//	else if ((col > 0) && (row > 0) && (row <= movetable.size()))
//	{
//		--row; // Row 0 is the header
//		if (col == hTheory.column)
//		{
//			if (movetable[row].rep[0])
//			{
//				contextMenu->addAction(QString("Remove from theory"), this, [=] {deleteMove(0, movetable[row].move); });
//			}
//		}
//		else if (col == hWhite.column)
//		{
//			if (movetable[row].rep[1])
//			{
//				contextMenu->addAction(QString("Remove from White repertoire"), this, [=] {deleteMove(1, movetable[row].move); });
//				if (movetable[row].rep[1] == 3)
//					contextMenu->addAction(QString("Set as main White repertoire move"), this, [=] {setMainMove(1, movetable[row].move); });
//			}
//		}
//		else if (col == hBlack.column)
//		{
//			if (movetable[row].rep[2])
//			{
//				contextMenu->addAction(QString("Remove from Black repertoire"), this, [=] {deleteMove(2, movetable[row].move); });
//				if (movetable[row].rep[2] == 3)
//					contextMenu->addAction(QString("Set as main White repertoire move"), this, [=] {setMainMove(2, movetable[row].move); });
//			}
//		}
//		else if (col == hComp.column)
//		{
//			contextMenu->addAction(QString("Set engine priority"), this, SLOT(rearrangeEngines()));
//		}
//	}
//	contextMenu->addAction(QString("Font"), this, SLOT(selectFont()));
//	contextMenu->exec(mapToGlobal(pos));
}

//void MoveTableWindow::moveClicked(const QModelIndex& index)
//{
//	int col = index.column();
//	int row = index.row();
//
//	if (col == hMoves.column)
//	{
//		if ((row>=0) && (row < movetable.size()))
//			emit moveSelected(movetable[row].move);
//	}
//}
//
//void MoveTableWindow::deleteMove(int rep, const ChessMove& move)
//{
//	emit moveDelete(rep, move);
//}
//
//void MoveTableWindow::setMainMove(int rep, const ChessMove& move)
//{
//	emit moveSetAsMain(rep, move);
//}
//
//void MoveTableWindow::rearrangeEngines()
//{
//	ListOrderDialog dialog(tr("Arrange engine priority"), this);
//	dialog.setList(computerDB->enginelist);
//	if (dialog.exec() == QDialog::Rejected)
//		return;
//	QStringList l = dialog.getList();
//	computerDB->enginelist = l;
//	computerDB->saveEngineList();
//	emit needRefresh();
//};
//
//void MoveTableWindow::addComment(QString& comment)
//{
//	//QItemSelectionModel* sel = table->selectionModel();
//	//if (sel->hasSelection())
//	//{
//	//	QModelIndex qmi = sel->currentIndex();
//	//	if (qmi.isValid())
//	//		emit addMoveComment(qmi.column(), qmi.row(), comment);
//	//}
//}
//
//QString MoveTableWindow::fontToString()
//{
//	return font().toString();
//}
//
//void MoveTableWindow::fontFromString(const QString& sFont)
//{
//	if (sFont.isEmpty())
//		return;
//	QFont f;
//	f.fromString(sFont);
//	setFont(f);
//	table->resizeColumnsToContents();
//}
