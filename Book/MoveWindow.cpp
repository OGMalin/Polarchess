#include "MoveWindow.h"
#include "CreateStatisticsDialog.h"
#include <QTableView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QStringList>
#include <QVector>
#include <QMenu>
#include <QFontDialog>
#include <QTextStream>

MoveWindow::MoveWindow(QWidget *parent)
	: QWidget(parent)
{
	computer = NULL;
	QVBoxLayout* vbox = new QVBoxLayout;
	model = new QStandardItemModel(0, 0);
	//QStringList h;
	//h << tr("Move") << tr("Theory") << tr("White") << tr("Black") << tr("Score") << tr("Games") << tr("Comp");
	//model->setHorizontalHeaderLabels(h);
	table = new QTableView;
	table->setModel(model);
	table->setSelectionMode(QAbstractItemView::SingleSelection);
	vbox->setMargin(0);
	vbox->addWidget(table);
	setLayout(vbox);
//	setContentsMargins(0, 0, 0, 0);
	normalBrush.setColor(Qt::black);
	repBrush.setColor(Qt::green);

	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));
	connect(table, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(moveClicked(const QModelIndex&)));
	font.setPointSize(12);
	this->setFont(font);
}

MoveWindow::~MoveWindow()
{
}

void MoveWindow::add(BookDBEntry& book, int rep, ChessBoard& cb)
{
	MoveTableEntry mte;
	QVector<BookDBMove>::iterator bit;
	int index,val;

	bit = book.movelist.begin();
	while (bit != book.movelist.end())
	{
		if (rep == 0)
		{
			val = 1;
		}
		else
		{
			if (cb.toMove == (rep - 1))
			{
				if (bit == book.movelist.begin())
					val = 2;
				else
					val = 3;
			}
			else
			{
				val = 1;
			}
		}
		index = existInTable(bit->move);

		if (index<0)
		{
			mte.clear();
			mte.move = bit->move;
			mte.rep[rep] = val;
			movetable.push_back(mte);
		}
		else
		{
			movetable[index].rep[rep] = val;
		}
		++bit;
	}
}

void MoveWindow::add(StatisticsDBEntry& stat)
{
	MoveTableEntry mte;
	QVector<StatisticsDBMove>::iterator sit;
	std::sort(stat.movelist.begin(), stat.movelist.end());
	sit = stat.movelist.begin();
	while (sit != stat.movelist.end())
	{
		mte.clear();
		mte.move = sit->move;
		mte.whitewin = sit->whitewin;
		mte.draw = sit->draw;
		mte.blackwin = sit->blackwin;
		mte.elo = sit->elo;
		mte.year = sit->year;
		movetable.push_back(mte);
		++sit;
	}
}

void MoveWindow::add(ComputerDBEntry& comp)
{
	MoveTableEntry mte;
	int index;
	QVector<ComputerDBEngine>::iterator cit;
	QVector<QString>::iterator eit;
	eit = computer->enginelist.begin();
	while (eit != computer->enginelist.end())
	{
		cit = comp.enginelist.begin();
		while (cit != comp.enginelist.end())
		{
			if (eit == cit->engine)
			{
				if (cit->pv.size)
				{
					index = existInTable(cit->pv.list[0]);
					if (index < 0)
					{
						mte.clear();
						mte.move = cit->pv.list[0];
						mte.cp = cit->cp;
						mte.engine = cit->engine;
						mte.depth = cit->depth;
						movetable.push_back(mte);
					}
					else if (movetable[index].engine.isEmpty())
					{
						movetable[index].cp = cit->cp;
						movetable[index].engine = cit->engine;
						movetable[index].depth = cit->depth;
					}
				}
			}
			++cit;
		}
		++eit;
	}
}

int MoveWindow::existInTable(ChessMove& m)
{
	int i;
	for (i = 0; i < movetable.size(); i++)
		if (movetable[i].move == m)
			return i;
	return -1;
}

void MoveWindow::refresh(BookDBEntry& theory, BookDBEntry& white, BookDBEntry& black, StatisticsDBEntry& stat, ComputerDBEntry& compdata, ChessBoard& cb)
{
	int i,j;
	int rows;
	QString qs;
	QStandardItem* item;
	movetable.clear();
	add(stat);
	if (computer)
		add(compdata);
	add(theory, 0, cb);
	add(white, 1, cb);
	add(black, 2, cb);

	model->setRowCount(0);

	for (i = 0; i < movetable.size(); i++)
	{
		// Moves
		qs = QString(cb.makeMoveText(movetable[i].move, tr("NBRQK").toStdString()).c_str());
		item = new QStandardItem(qs);
		model->setItem(i, 0, item);

		// Theory
		if (movetable[i].rep[0])
		{
			item = new QStandardItem();
			item->setBackground(QBrush(QColor("gray")));
			model->setItem(i, 1, item);
		}

		// White repertoire
		if (movetable[i].rep[1])
		{
			item = new QStandardItem();
			if (movetable[i].rep[1]==1)
				item->setBackground(QBrush(QColor("gray")));
			else if (movetable[i].rep[1] == 2)
				item->setBackground(QBrush(QColor("green")));
			else if (movetable[i].rep[1] == 3)
					item->setBackground(QBrush(QColor("yellow")));
			model->setItem(i, 2, item);
		}

		// Black repertoire
		if (movetable[i].rep[2])
		{
			item = new QStandardItem();
			if (movetable[i].rep[2] == 1)
				item->setBackground(QBrush(QColor("gray")));
			else if (movetable[i].rep[2] == 2)
					item->setBackground(QBrush(QColor("green")));
			else if (movetable[i].rep[2] == 3)
						item->setBackground(QBrush(QColor("yellow")));
			model->setItem(i, 3, item);
		}

		// Score
		if ((movetable[i].whitewin + movetable[i].draw + movetable[i].blackwin) > 0)
		{
			qs.clear();
			QTextStream(&qs) << movetable[i].whitewin << "/" << movetable[i].draw << "/" << movetable[i].blackwin;
			item = new QStandardItem(qs);
			model->setItem(i, 4, item);

			// Games
			qs.clear();
			QTextStream(&qs) << movetable[i].whitewin + movetable[i].draw + movetable[i].blackwin;
			item = new QStandardItem(qs);
			model->setItem(i, 5, item);
		}

		// Comp
		if (!movetable[i].engine.isEmpty())
		{
			qs.sprintf("%i", movetable[i].cp);
			item = new QStandardItem(qs);
			qs.clear();
			QTextStream(&qs) << movetable[i].engine << endl << "Depth: " << movetable[i].depth;
			item->setToolTip(qs);
			model->setItem(i, 6, item);
		}

	}

	QStringList label;
	for (i = 0; i < movetable.size(); i++)
	{
		if (cb.toMove == BLACK)
			label.push_back("...");
		else
			label.push_back("");
	}
	model->setVerticalHeaderLabels(label);
	model->setColumnCount(7);
	model->setRowCount(movetable.size());
}

void MoveWindow::showContextMenu(const QPoint& pos)
{
	QMenu* contextMenu = new QMenu(this);
	QModelIndex qmi = table->indexAt(pos);
	if (qmi.row() == 0)
	{
		contextMenu->addAction(QString("v"), this, SLOT(addComment0()));

	}
	//contextMenu->addAction(QString("No comment"), this, SLOT(addComment0()));
	//contextMenu->addAction(QString("!"), this, SLOT(addComment1()));
	//contextMenu->addAction(QString("?"), this, SLOT(addComment2()));
	//contextMenu->addAction(QString("!!"), this, SLOT(addComment3()));
	//contextMenu->addAction(QString("??"), this, SLOT(addComment4()));
	//contextMenu->addAction(QString("!?"), this, SLOT(addComment5()));
	//contextMenu->addAction(QString("?!"), this, SLOT(addComment6()));
	//contextMenu->addAction(QString("Delete move"), this, SLOT(deleteMove()));
	//contextMenu->addAction(QString("Font"), this, SLOT(selectFont()));
	contextMenu->exec(mapToGlobal(pos));
}

void MoveWindow::selectFont()
{
	bool ok;
	QFont f = QFontDialog::getFont(&ok, font, this);
	if (ok)
	{
		font = f;
		this->setFont(font);
	}
}

void MoveWindow::moveClicked(const QModelIndex& index)
{
//	emit moveSelected(index.column(), index.row());
}

void MoveWindow::deleteMove()
{
	//QItemSelectionModel* sel = table->selectionModel();
	//if (sel->hasSelection())
	//{
	//	QModelIndex qmi = sel->currentIndex();
	//	if (qmi.isValid())
	//		emit moveDelete(qmi.column(), qmi.row());
	//}
}

void MoveWindow::addComment(QString& comment)
{
	//QItemSelectionModel* sel = table->selectionModel();
	//if (sel->hasSelection())
	//{
	//	QModelIndex qmi = sel->currentIndex();
	//	if (qmi.isValid())
	//		emit addMoveComment(qmi.column(), qmi.row(), comment);
	//}
}
