#pragma once

#include <QListWidget>

class QChessGame;

class PathWindow : public QListWidget
{
	Q_OBJECT

public:
	PathWindow(QWidget *parent = 0);
	~PathWindow();
	void update(QChessGame* game);
signals:
	void pathSelected(int);
public slots:
	void moveClicked(QListWidgetItem*);
private:
	QChessGame* currentGame;
};

