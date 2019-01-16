#pragma once

#include <QListWidget>

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
};

