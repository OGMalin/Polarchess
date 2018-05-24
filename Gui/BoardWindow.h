#pragma once

#include <QWidget>
//#include <QTextEdit>
#include "../Common/ChessBoard.h"

class BoardWindow :public QWidget
{
	Q_OBJECT
public:
	BoardWindow(QWidget* parent = 0);
protected:
	void paintEvent(QPaintEvent* event) override;
private:
	ChessBoard currentBoard;
	int squareSize;
	QBrush lightBrush;
	QBrush darkBrush;
	QBrush borderBrush;

	void setBoardStyle();
};