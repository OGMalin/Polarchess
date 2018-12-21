#pragma once

#include "Database.h"
#include <QTextEdit>

class CommentWindow : public QTextEdit
{
	Q_OBJECT

public:
	CommentWindow(QWidget *parent=0);
	~CommentWindow();
	void update(BookDBEntry& theory, BookDBEntry& rep);
};
