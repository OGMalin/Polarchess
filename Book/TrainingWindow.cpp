#include "TrainingWindow.h"

TrainingWindow::TrainingWindow(QWidget* parent)
	: QWidget(parent)
{
	setContextMenuPolicy(Qt::CustomContextMenu);
//	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));
}

TrainingWindow::~TrainingWindow()
{
}

QString TrainingWindow::fontToString()
{
	return font().toString();
}

void TrainingWindow::fontFromString(const QString& sFont)
{
	QFont f;
	f.fromString(sFont);
	setFont(f);
}
