#pragma once

#include <QTextEdit>

class CommentWindow : public QTextEdit
{
	Q_OBJECT

public:
	CommentWindow(QWidget *parent=0);
	~CommentWindow();
};
