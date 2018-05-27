#pragma once

#include <QWidget>
#include <QPixmap>
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
	void flip();
	void flip(bool inv);
	void newGame();
protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
private:
	ChessBoard currentBoard;
	bool whiteAtBottom;
	int squareSize;
	QRect boardRect;
	QRect borderRect;
	BOARDTHEMA theme;
	typePiece dragPiece;
	QRect dragPieceRect;
	int dragFromSquare;
	void drawBorder(QPaintEvent* event, QPainter& painter);
	void drawCoordinates(QPaintEvent* event, QPainter& painter);
	void drawBoard(QPaintEvent* event, QPainter& painter);
	void drawPieces(QPaintEvent* event, QPainter& painter);
	void drawDragPiece(QPaintEvent* event, QPainter& painter);
	void drawSquare(QPaintEvent* event, QPainter& painter, int sq, bool clear = false);
	int findSquare(const QPoint& point);
	void setBoardTheme();
	void calculateBoard();
	QRect squareRect(int sq);

	// Handling pieces
	QPixmap imgPieces[12];
	void readImgPieces();
};