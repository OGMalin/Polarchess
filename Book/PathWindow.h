#pragma once

#include <QWidget>
#include "../Common/QChessGame.h"

class PathWindow : public QWidget
{
	Q_OBJECT

public:
	PathWindow(QWidget *parent = 0);
	~PathWindow();
	void update(QChessGame& game);

private:

};

