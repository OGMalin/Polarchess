#include "MoveWindow.h"
#include "CreateStatisticsDialog.h"
#include <QTableView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QStringList>
#include <QVector>
#include <QMenu>
#include <QFontDialog>
#include <QApplication>
#include <QStandardPaths>

MoveWindow::MoveWindow(QWidget *parent)
	: QWidget(parent)
{
	QString spath = QStandardPaths::locate(QStandardPaths::DocumentsLocation, QCoreApplication::organizationName(), QStandardPaths::LocateDirectory);
	spath += "/" + QCoreApplication::applicationName();
	statistics = new Statistics();
	if (!statistics->open(spath + "/Statistics.bin"))
		statistics->create(spath + "/Statistics.bin");
	QVBoxLayout* vbox = new QVBoxLayout;
	model = new QStandardItemModel(0, 3);
	QStringList header;
	header << "Theory" << "White" << "Black";
	model->setHorizontalHeaderLabels(header);
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

void MoveWindow::refresh(BookDBEntry& theory, BookDBEntry& white, BookDBEntry& black)
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
	for (i = 0; i < white.movelist.size(); i++)
	{
		qs = QString(white.board.makeMoveText(white.movelist[i].move, FIDE).c_str());
		qs += white.movelist[i].comment;
		item = new QStandardItem(qs);
		item->setEditable(false);
		item->setTextAlignment(Qt::AlignCenter);
		if (white.board.toMove==WHITE)
			item->setForeground(repBrush);
		else
			item->setForeground(normalBrush);
		model->setItem(i, 1, item);
	}
	rows = __max(rows,i);
	for (i = 0; i < black.movelist.size(); i++)
	{
		qs = QString(black.board.makeMoveText(black.movelist[i].move, FIDE).c_str());
		qs += black.movelist[i].comment;
		item = new QStandardItem(qs);
		item->setEditable(false);
		item->setTextAlignment(Qt::AlignCenter);
		if (black.board.toMove == BLACK)
			item->setForeground(repBrush);
		else
			item->setForeground(normalBrush);
		model->setItem(i, 2, item);
	}
	rows = __max(rows, i);

	model->setRowCount(rows);
}

void MoveWindow::showContextMenu(const QPoint& pos)
{
	QMenu* contextMenu = new QMenu(this);
	QModelIndex qmi = table->indexAt(pos);
	contextMenu->addAction(QString("No comment"), this, SLOT(addComment0()));
	contextMenu->addAction(QString("!"), this, SLOT(addComment1()));
	contextMenu->addAction(QString("?"), this, SLOT(addComment2()));
	contextMenu->addAction(QString("!!"), this, SLOT(addComment3()));
	contextMenu->addAction(QString("??"), this, SLOT(addComment4()));
	contextMenu->addAction(QString("!?"), this, SLOT(addComment5()));
	contextMenu->addAction(QString("?!"), this, SLOT(addComment6()));
	contextMenu->addAction(QString("Delete move"), this, SLOT(deleteMove()));
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

void MoveWindow::moveClicked(const QModelIndex& index)
{
	emit moveSelected(index.column(), index.row());
}

void MoveWindow::deleteMove()
{
	QItemSelectionModel* sel = table->selectionModel();
	if (sel->hasSelection())
	{
		QModelIndex qmi = sel->currentIndex();
		if (qmi.isValid())
			emit moveDelete(qmi.column(), qmi.row());
	}
}

void MoveWindow::addComment(QString& comment)
{
	QItemSelectionModel* sel = table->selectionModel();
	if (sel->hasSelection())
	{
		QModelIndex qmi = sel->currentIndex();
		if (qmi.isValid())
			emit addMoveComment(qmi.column(), qmi.row(), comment);
	}
}

void MoveWindow::createStatistics()
{
	statistics->importGames(this);
}