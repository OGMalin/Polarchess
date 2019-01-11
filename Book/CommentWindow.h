#pragma once

#include "Database.h"
#include <QWidget>
#include <QColor>
#include <QFont>

class QTextEdit;

class CommentWindow : public QWidget
{
	Q_OBJECT

public:
	CommentWindow(QWidget *parent=0);
	~CommentWindow();
	void update(BookDBEntry& theory, BookDBEntry& rep);
private:
	QTextEdit* comment;
	QColor theoryColor;
	QColor repColor;
	QFont font;
};
