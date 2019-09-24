#include "MoveWindow.h"
#include <QTableView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QStringList>
#include <QVector>
#include <QMenu>
#include <QFontDialog>

MoveWindow::MoveWindow(QWidget *parent)
	: QWidget(parent)
{
	QVBoxLayout* vbox = new QVBoxLayout;
	model = new QStandardItemModel(0, 2);
	QStringList header;
	header << "Theory" << "Repertoire";
	model->setHorizontalHeaderLabels(header);
	table = new QTableView;
	table->setModel(model);
	vbox->setMargin(0);
	vbox->addWidget(table);
	setLayout(vbox);
//	setContentsMargins(0, 0, 0, 0);
	normalBrush.setColor(Qt::black);
	repBrush.setColor(Qt::green);

	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));

	font.setPointSize(12);
	this->setFont(font);
}

MoveWindow::~MoveWindow()
{
}

void MoveWindow::update(BookDBEntry& theory, BookDBEntry& white, BookDBEntry& black)
{
	int i,j;
	QString qs;
	QStandardItem* item;
	int rows;
	model->setRowCount(0);
	for (i = 0; i < theory.movelist.size();i++)
	{
		qs=QString(theory.board.makeMoveText(theory.movelist[i].move, FIDE).c_str());
		qs += theory.movelist[i].comment;
		item = new QStandardItem(qs);
		item->setEditable(false);
		item->setTextAlignment(Qt::AlignCenter);
		model->setItem(i,0,item);
	}
	rows = i;
	BookDBEntry bde = white;
	QVector<BookDBMove> bms = black.movelist;
	BookDBMove bm;
	for (i = 0; i < bde.movelist.size(); i++)
	{
		if (bde.board.toMove == WHITE)
			bde.movelist[i].score = 1;
		else
			bde.movelist[i].score = 0;
	}
	for (i = 0; i < bms.size(); i++)
	{
		if (bde.moveExist(bms[i].move))
		{
			if (bde.board.toMove == BLACK)
			{
				bms[i].score = 2;
				bde.updateMove(bms[i]);
			}
		}else
		{
			if (bde.board.toMove == BLACK)
				bms[i].score = 2;
			else
				bms[i].score = 0;
			bde.movelist.append(bms[i]);
		}
	}
	for (i = 0; i < bde.movelist.size(); i++)
	{
		qs = QString(bde.board.makeMoveText(bde.movelist[i].move, FIDE).c_str());
		qs += bde.movelist[i].comment;
		item = new QStandardItem(qs);
		item->setEditable(false);
		item->setTextAlignment(Qt::AlignCenter);
		if (bde.movelist[i].score>0)
			item->setForeground(repBrush);
		else
			item->setForeground(normalBrush);

		for (j = 0; j < theory.movelist.size(); j++)
		{
			if (bde.movelist[i].move == theory.movelist[j].move)
				break;
		}
		if (j < theory.movelist.size())
		{
			model->setItem(j, 1, item);
		}
		else
		{
			model->setItem(rows, 1, item);
			++rows;
		}
	}
	model->setRowCount(rows);
}

void MoveWindow::showContextMenu(const QPoint& pos)
{
	QMenu* contextMenu = new QMenu(this);
	contextMenu->addAction(QString("Font"), this, SLOT(selectFont()));
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
