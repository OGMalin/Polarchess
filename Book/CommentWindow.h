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
	CommentWindow(QWidget *parent=0);
	~CommentWindow();
	void update(QString theory, QString rep);
	void setWriteTheory(bool b);
	void setWriteRep(bool b);
	void disableWrite();

signals:
	void commentChanged(QString&);

private:
	QTextEdit* comment;
	QColor theoryColor;
	QColor repColor;
	QFont font;
	bool writeTheory, writeRep;
	QString cTheory, cRep;
protected:
	virtual void mouseDoubleClickEvent(QMouseEvent* event);
};
