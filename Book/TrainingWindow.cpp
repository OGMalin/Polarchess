#include "TrainingWindow.h"
#include <QMenu>
#include <QFontDialog>

TrainingWindow::TrainingWindow(QWidget* parent)
	: QWidget(parent)
{

	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));
}

TrainingWindow::~TrainingWindow()
{
}

void TrainingWindow::showContextMenu(const QPoint& pos)
{
	QMenu* contextMenu = new QMenu(this);
	contextMenu->addAction(QString("Font"), this, SLOT(selectFont()));
	contextMenu->exec(mapToGlobal(pos));
}

void TrainingWindow::selectFont()
{
	bool ok;
	QFont f = QFontDialog::getFont(&ok, font(), this);
	if (ok)
	{
		setFont(f);
	}
}

QString TrainingWindow::fontToString()
{
	return font().toString();
}

void TrainingWindow::fontFromString(const QString& sFont)
{
	if (sFont.isEmpty())
		return;
	QFont f;
	f.fromString(sFont);
	setFont(f);
}
