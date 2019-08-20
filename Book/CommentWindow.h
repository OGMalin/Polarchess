#pragma once

#include <QWidget>
#include <QColor>
#include <QFont>
#include <QString>

class QTextEdit;

class CommentWindow : public QWidget
{
	Q_OBJECT

public:
	int write;
	CommentWindow(QWidget *parent=0);
	~CommentWindow();
	void update(QString theory, QString white, QString black);
	void disableWrite() { write = -1; };

signals:
	void commentChanged(QString&);

private:
	QTextEdit* comment;
	QColor theoryColor;
	QColor repColor;
	QFont font;
	QString cTheory, cWhite, cBlack;
protected:
	virtual void mouseDoubleClickEvent(QMouseEvent* event);
};
