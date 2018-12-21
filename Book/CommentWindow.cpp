#include "CommentWindow.h"

CommentWindow::CommentWindow(QWidget *parent)
	: QTextEdit(parent)
{
}

CommentWindow::~CommentWindow()
{
}

void CommentWindow::update(BookDBEntry& theory, BookDBEntry& rep)
{
	setText(theory.comment+"\n"+rep.comment);
}
