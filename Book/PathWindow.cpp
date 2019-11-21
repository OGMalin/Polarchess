#include "PathWindow.h"
#include "Path.h"
#include <QStringList>
#include <QMenu>
#include <QFontDialog>

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

	font.setPointSize(12);
	this->setFont(font);
}

PathWindow::~PathWindow()
{
}

void PathWindow::update(Path* path)
{
	char sz[16];
	QStringList ml;
	QString p = ".";
	clear();
	path->getMoveList(ml);
	addItem("O"); // Start position
	for (int i = 0; i < ml.size(); i++)
	{
		if (!(i % 2))
			addItem(itoa(i / 2 + 1, sz, 10) + p + ml[i]);
		else
			addItem(ml[i]);
	}
}

void PathWindow::moveClicked(QListWidgetItem* item)
{
	emit pathSelected(currentRow());
}

void PathWindow::showContextMenu(const QPoint& pos)
{
	QMenu* contextMenu = new QMenu(this);
	contextMenu->addAction(QString("Add line to theory"), this, SLOT(addPathT()));
	contextMenu->addAction(QString("Add line to White repertoire"), this, SLOT(addPathW()));
	contextMenu->addAction(QString("Add line to Black repertoire"), this, SLOT(addPathT()));
	contextMenu->addAction(QString("Font"), this, SLOT(selectFont()));
	contextMenu->exec(mapToGlobal(pos));
}

void PathWindow::selectFont()
{
	bool ok;
	QFont f = QFontDialog::getFont(&ok, font, this);
	if (ok)
	{
		font = f;
		this->setFont(font);
	}
}

void PathWindow::addPath(int rep)
{
	emit pathToDB(rep);
}