#pragma once

#include <QWidget>
//#include <QTextEdit>
#include "../Common/ChessBoard.h"

struct BOARDTHEMA
{
	QBrush lightBrush;
	QBrush darkBrush;
	QBrush borderBrush;
	QFont  coordinateFont;
	QColor coordinateFontColor;
};

class BoardWindow :public QWidget
{
	Q_OBJECT
public:
	BoardWindow(QWidget* parent = 0);
protected:
	void paintEvent(QPaintEvent* event) override;
private:
	ChessBoard currentBoard;
	bool whiteAtBottom;
	int squareSize;
	QRect boardRect;
	QRect borderRect;
	BOARDTHEMA theme;

	void drawBorder(QPaintEvent* event, QPainter& painter);
	void drawCoordinates(QPaintEvent* event, QPainter& painter);
	void drawBoard(QPaintEvent* event, QPainter& painter);
	void setBoardTheme();
	void calculateBoard();
	QRect squareRect(int sq);
};