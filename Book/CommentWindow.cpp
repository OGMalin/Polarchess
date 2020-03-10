#include "CommentWindow.h"
#include <QTextEdit>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QMenu>
#include <QFontDialog>

CommentWindow::CommentWindow(QWidget *parent)
	: QWidget(parent)
{
	write = -1;
	cTheory.clear();
	cWhite.clear();
	cBlack.clear();
	QVBoxLayout* vbox = new QVBoxLayout;
	comment = new QTextEdit;
	comment->setReadOnly(true);
	vbox->setMargin(0);
	vbox->addWidget(comment);
	setLayout(vbox);
	theoryColor.setRgb(0, 0, 128);
	repColor.setRgb(0, 128, 0);

	comment->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(comment, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));
}

CommentWindow::~CommentWindow()
{
}

void CommentWindow::refresh(QString theory, QString white, QString black)
{
	cTheory = theory;
	cWhite = white;
	cBlack = black;
	comment->clear();
	if (!cWhite.isEmpty())
	{
		comment->setTextColor(repColor);
		comment->append(cWhite);
	}
	if (!cBlack.isEmpty())
	{
		comment->setTextColor(repColor);
		comment->append(cBlack);
	}
	if (!cTheory.isEmpty())
	{
		comment->setTextColor(theoryColor);
		comment->append(cTheory);
	}
}

void CommentWindow::mouseDoubleClickEvent(QMouseEvent* event)
{
	switch (write)
	{
		case 0:
			editTheory();
			break;
		case 1:
			editWhite();
			break;
		case 2:
			editBlack();
			break;
		default:
			break;
	}
}

void CommentWindow::showContextMenu(const QPoint& pos)
{
	QMenu* contextMenu = new QMenu(this);
	contextMenu->addAction(tr("Edit theory comment"), this, &CommentWindow::editTheory);
	contextMenu->addAction(tr("Edit White repertoire comment"), this, &CommentWindow::editWhite);
	contextMenu->addAction(tr("Edit Black repertoire comment"), this, &CommentWindow::editBlack);
	contextMenu->addSeparator();
	contextMenu->addAction(tr("Font"), this, &CommentWindow::selectFont);
	contextMenu->exec(mapToGlobal(pos));
}

void CommentWindow::selectFont()
{
	bool ok;
	QFont f = QFontDialog::getFont(&ok, font(), this);
	if (ok)
	{
		this->setFont(f);
	}
}

QString CommentWindow::fontToString()
{
	return font().toString();
}

void CommentWindow::fontFromString(const QString& sFont)
{
	if (sFont.isEmpty())
		return;
	QFont f;
	f.fromString(sFont);
	setFont(f);
	//	table->resizeColumnsToContents();
}

void CommentWindow::editTheory()
{
	bool res = false;
	QString newText;
	newText = QInputDialog::getMultiLineText(this, "PolarBook", "Theory book comment", cTheory, &res);
	if (res && (newText != cTheory))
	{
		cTheory = newText;
		refresh(cTheory, cWhite, cBlack);
		emit commentChanged(cTheory, 0);
	}
}

void CommentWindow::editWhite()
{
	bool res = false;
	QString newText;
	newText = QInputDialog::getMultiLineText(this, "PolarBook", "White repertoire comment", cWhite, &res);
	if (res && (newText != cWhite))
	{
		cWhite = newText;
		refresh(cTheory, cWhite, cBlack);
		emit commentChanged(cWhite, 1);
	}
}

void CommentWindow::editBlack()
{
	bool res = false;
	QString newText;
	newText = QInputDialog::getMultiLineText(this, "PolarBook", "White repertoire comment", cBlack, &res);
	if (res && (newText != cBlack))
	{
		cBlack = newText;
		refresh(cTheory, cWhite, cBlack);
		emit commentChanged(cBlack, 2);
	}
}
