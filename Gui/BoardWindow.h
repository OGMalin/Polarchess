#pragma once

#include <QWidget>
#include "../Common/ChessBoard.h"

class BoardWindow :public QWidget
{
	Q_OBJECT
public:
	ChessBoard currentBoard;
	BoardWindow(QWidget* parent);
};