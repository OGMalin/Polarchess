#include "OpeningWindow.h"
#include <QHBoxLayout>
#include <QMenu>
#include <QFontDialog>
#include <QInputDialog>
#include "OpeningsDialog.h"

OpeningWindow::OpeningWindow(QWidget *parent)
	: QWidget(parent)
{
	openingsDB = NULL;
	QHBoxLayout* hbox = new QHBoxLayout;
	openingline = new QLineEdit;
	openingline->setDisabled(true);
	hbox->addWidget(openingline);
	setLayout(hbox);

	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));
}

OpeningWindow::~OpeningWindow()
{
}

void OpeningWindow::refresh(Path* path)
{
	if (!openingsDB)
		return;
	PathEntry pe;
	currentPosition = CompressedBoard::compress(path->getPosition());

	int i = path->current();
	ode.clear();
	while (i >= 0)
	{
		pe = path->getEntry(i);
		ode = openingsDB->find(pe.board);
		if (ode.cBoard.size())
			break;
		--i;
	}

	QString qs;
	if (ode.eco.size())
	{
		qs += ode.eco;
		if (ode.name.size())
			qs += ": ";
	}
	if (ode.name.size())
	{
		qs += ode.name;
		if (ode.variation.size())
		{
			qs += ", ";
			qs += ode.variation;
			if (ode.subvariation.size())
			{
				qs += ", ";
				qs += ode.subvariation;
			}
		}
	}
	openingline->setText(qs);
}

void OpeningWindow::showContextMenu(const QPoint& pos)
{
	QMenu* contextMenu = new QMenu(this);
	contextMenu->addAction(QString("Edit"), this, SLOT(editLine()));
	contextMenu->addAction(QString("Remove"), this, SLOT(deleteLine()));
	contextMenu->addSeparator();
	contextMenu->addAction(QString("Font"), this, SLOT(selectFont()));
	contextMenu->exec(mapToGlobal(pos));
}

void OpeningWindow::selectFont()
{
	bool ok;
	QFont f = QFontDialog::getFont(&ok, font(), this);
	if (ok)
	{
		setFont(f);
	}
}

QString OpeningWindow::fontToString()
{
	return font().toString();
}

void OpeningWindow::fontFromString(const QString& sFont)
{
	if (sFont.isEmpty())
		return;
	QFont f;
	f.fromString(sFont);
	setFont(f);
}

void OpeningWindow::editLine()
{
	if (!openingsDB)
		return;

	OpeningsDialog dialog(this, ode.eco, ode.name, ode.variation, ode.subvariation, ode.cBoard.isEmpty());
	if (dialog.exec() == QDialog::Accepted)
	{
		dialog.getItems(ode.eco, ode.name, ode.variation, ode.subvariation);
		ode.cBoard = currentPosition;
		openingsDB->update(ode);
		QString qs;
		if (ode.eco.size())
		{
			qs += ode.eco;
			if (ode.name.size())
				qs += ": ";
		}
		if (ode.name.size())
		{
			qs += ode.name;
			if (ode.variation.size())
			{
				qs += ", ";
				qs += ode.variation;
				if (ode.subvariation.size())
				{
					qs += ", ";
					qs += ode.subvariation;
				}
			}
		}
		openingline->setText(qs);
	}
}

void OpeningWindow::deleteLine()
{
	openingsDB->remove(currentPosition);
}