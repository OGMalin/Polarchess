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
	CommentWindow(QWidget *parent = 0);
	~CommentWindow();
	void refresh(QString theory, QString white, QString black);
	void disableWrite() { write = -1; };
	QString fontToString();
	void fontFromString(const QString&);

signals:
	void commentChanged(QString&, int rep);

public slots:
	void showContextMenu(const QPoint& pos);

private:
	QTextEdit* comment;
	QColor theoryColor;
	QColor repColor;
	QString cTheory, cWhite, cBlack;
protected:
	virtual void mouseDoubleClickEvent(QMouseEvent* event);
	void editTheory();
	void editWhite();
	void editBlack();
	void selectFont();
};
