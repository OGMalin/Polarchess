#include "CommentWindow.h"
#include <QTextEdit>
#include <QVBoxLayout>
#include <QInputDialog>

CommentWindow::CommentWindow(QWidget *parent)
	: QWidget(parent)
{
	write = -1;
	cTheory.clear();
	cWhite.clear();
	cBlack.clear();
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

void CommentWindow::update(QString theory, QString white, QString black)
{
	cTheory = theory;
	cWhite = white;
	cBlack = black;
	comment->clear();
	comment->setFont(font);
	if (!cWhite.isEmpty())
	{
		comment->setTextColor(repColor);
		comment->append(cWhite);
	}
	if (!cWhite.isEmpty())
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
	bool res = false;
	QString newText;

	switch (write)
	{
		case 0:
			newText = QInputDialog::getMultiLineText(this, "PolarBook", "Theory comment", cTheory, &res);
			if (res)
				cTheory = newText;
			break;
		case 1:
			newText = QInputDialog::getMultiLineText(this, "PolarBook", "White repertoire comment", cWhite, &res);
			if (res)
				cWhite = newText;
			break;
		case 2:
			newText = QInputDialog::getMultiLineText(this, "PolarBook", "Black repertoire comment", cBlack, &res);
			if (res)
				cBlack = newText;
			break;
		default:
			return;
	}
	if (res)
	{
		update(cTheory, cWhite, cBlack);
		emit commentChanged(newText);
	}
}