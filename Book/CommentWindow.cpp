#include "CommentWindow.h"

CommentWindow::CommentWindow(QWidget *parent)
	: QTextEdit(parent)
{
}

CommentWindow::~CommentWindow()
{
}

void CommentWindow::update(BookDBEntry& data)
{
	setText(data.comment);
}
