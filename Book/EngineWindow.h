#pragma once

#include <QWidget>
#include "../Common/ChessBoard.h"

class EngineWindow : public QWidget
{
	Q_OBJECT

public:
	EngineWindow(QWidget *parent = 0);
	~EngineWindow();
	void update(ChessBoard& board);

private:

};

