#pragma once

#include <QWidget>
#include <QListWidget>
#include <QPoint>
#include <QFont>

class Path;

class PathWindow : public QListWidget
{
	Q_OBJECT

public:
	PathWindow(QWidget *parent = 0);
	~PathWindow();
	void update(Path* path);

signals:
	void pathSelected(int);

public slots:
	void moveClicked(QListWidgetItem*);
	void showContextMenu(const QPoint& pos);
	void selectFont();

private:
	QFont font;
};

