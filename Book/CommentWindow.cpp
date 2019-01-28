#include "CommentWindow.h"
#include <QTextEdit>
#include <QVBoxLayout>
#include <QInputDialog>

CommentWindow::CommentWindow(QWidget *parent)
	: QWidget(parent)
{
	writeTheory = false;
	writeRep = false;
	cTheory.clear();
	cRep.clear();
	QVBoxLayout* vbox = new QVBoxLayout;
	comment = new QTextEdit;
	comment->setDisabled(true);
	vbox->setMargin(0);
	vbox->addWidget(comment);
	setLayout(vbox);
	theoryColor.setRgb(0, 0, 128);
	repColor.setRgb(0, 128, 0);
	font.setPointSize(12);
}

CommentWindow::~CommentWindow()
{
}

void CommentWindow::update(QString theory, QString rep)
{
	cTheory = theory;
	cRep = rep;
	comment->clear();
	comment->setFont(font);
	if (!cRep.isEmpty())
	{
		comment->setTextColor(repColor);
		comment->append(cRep);
	}
	if (!cTheory.isEmpty())
	{
		comment->setTextColor(theoryColor);
		comment->append(cTheory);
	}
}

void CommentWindow::setWriteTheory(bool b)
{
	writeTheory = b;
	writeRep = false;
}

void CommentWindow::setWriteRep(bool b)
{
	writeTheory = false;
	writeRep = b;
}

void CommentWindow::disableWrite()
{
	writeTheory = false;
	writeRep = false;
}

void CommentWindow::mouseDoubleClickEvent(QMouseEvent* event)
{
	bool res;

	if (writeTheory)
	{
		QString newText = QInputDialog::getMultiLineText(this, "PolarBook", "Theory comment", cTheory, &res);
		if (res)
		{
			cTheory = newText;
			update(cTheory, cRep);
			emit commentChanged(cTheory);
		}
	}
	else if (writeRep)
	{
		QString newText = QInputDialog::getMultiLineText(this, "PolarBook", "Repertoire comment", cRep, &res);
		if (res)
		{
			cRep = newText;
			update(cTheory, cRep);
			emit commentChanged(cRep);
		}
	}
}