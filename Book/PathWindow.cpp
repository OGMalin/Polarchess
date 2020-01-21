#include "PathWindow.h"
#include "Path.h"
#include <QStringList>
#include <QMenu>
#include <QFontDialog>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>

PathWindow::PathWindow(QWidget *parent)
	: QListWidget(parent)
{
	char sz[16];
	setViewMode(QListWidget::IconMode);
	setResizeMode(QListWidget::Adjust);
	setMovement(QListWidget::Static);

	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(moveClicked(QListWidgetItem*)));
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));

	normalBrush.setColor(Qt::black);
	grayedBrush.setColor(Qt::gray);
}

PathWindow::~PathWindow()
{
}

void PathWindow::refresh(Path* path)
{
	char sz[16];
	QStringList ml;
	QString p = ".";
	clear();
	path->getMoveList(ml,tr("NBRQK").toLatin1());
	QListWidgetItem* lwi = new QListWidgetItem(QIcon(":/icon/home48.png"), QString());
	addItem(lwi);

//	addItem("O"); // Start position
	for (int i = 0; i < ml.size(); i++)
	{
		if (!(i % 2))
			addItem(itoa(i / 2 + 1, sz, 10) + p + ml[i]);
		else
			addItem(ml[i]);
		if (i >= path->current())
			item(count() - 1)->setForeground(grayedBrush);
		else
			item(count() - 1)->setForeground(normalBrush);
	}
}

void PathWindow::moveClicked(QListWidgetItem* item)
{
	emit pathSelected(currentRow());
}

void PathWindow::showContextMenu(const QPoint& pos)
{
	const QClipboard *clipboard = QApplication::clipboard();
	const QMimeData *mimeData = clipboard->mimeData();
	bool hasText = mimeData->hasText();

	QMenu* contextMenu = new QMenu(this);
	contextMenu->addAction(QString("Add line to theory"), this, SLOT(addPathT()));
	contextMenu->addAction(QString("Add line to White repertoire"), this, SLOT(addPathW()));
	contextMenu->addAction(QString("Add line to Black repertoire"), this, SLOT(addPathB()));
	contextMenu->addSeparator();
	contextMenu->addAction(QString("Copy"), this, SLOT(copy()));
	QAction* act=contextMenu->addAction(QString("Paste"), this, SLOT(paste()));
	if (!hasText)
		act->setDisabled(true);
	contextMenu->addSeparator();
	contextMenu->addAction(QString("Font"), this, SLOT(selectFont()));
	contextMenu->exec(mapToGlobal(pos));
}

void PathWindow::selectFont()
{
	bool ok;
	QFont f = QFontDialog::getFont(&ok, font(), this);
	if (ok)
	{
		setFont(f);
	}
}

void PathWindow::addPath(int rep)
{
	emit pathToDB(rep);
}

void PathWindow::copy()
{
	emit pathCopy();
}

void PathWindow::paste()
{
	emit pathPaste();
}

QString PathWindow::fontToString()
{
	return font().toString();
}

void PathWindow::fontFromString(const QString& sFont)
{
	if (sFont.isEmpty())
		return;
	QFont f;
	f.fromString(sFont);
	setFont(f);
	//	table->resizeColumnsToContents();
}
