#include "CommentWindow.h"
#include <QTextEdit>
#include <QVBoxLayout>

CommentWindow::CommentWindow(QWidget *parent)
	: QWidget(parent)
{
	QVBoxLayout* vbox = new QVBoxLayout;
	comment = new QTextEdit;
	vbox->addWidget(comment);
	setLayout(vbox);
	theoryColor.setRgb(255, 0, 0);
	repColor.setRgb(0, 255, 0);
	font.setPointSize(12);
}

CommentWindow::~CommentWindow()
{
}

void CommentWindow::update(BookDBEntry& theory, BookDBEntry& rep)
{

//	comment->setText(+"\n"+);
	comment->clear();
	comment->setFont(font);
	if (!rep.comment.isEmpty())
	{
		comment->setTextColor(repColor);
		comment->append(rep.comment);
	}
	if (!theory.comment.isEmpty())
	{
		comment->setTextColor(theoryColor);
		comment->append(theory.comment);
	}

}
