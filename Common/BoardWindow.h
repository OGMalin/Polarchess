#pragma once

#include <QWidget>
#include <QPixmap>
#include <QPoint>
#include <QColor>
#include "ChessBoard.h"
#include "ChessMove.h"

struct BOARDTHEMA
{
	QBrush lightBrush;
	QBrush darkBrush;
	QBrush borderBrush;
	QFont  coordinateFont;
	QColor coordinateFontColor;
};

struct BoardWindowMarkupArrow
{
	int fromSquare;
	int toSquare;
	QColor color;
	int timeout;
};

struct BoardWindowMarkupSquare
{
	int square;
	QColor color;
	int timeout;
};

class BoardWindow :public QWidget
{
	Q_OBJECT
public:
	BoardWindow(QWidget* parent = 0);
	void flip(bool inv);
	void newGame();
	void setPosition(const QString& fen);
	void setPosition(const ChessBoard& cb);
	void markSquare(int sq, QColor color, int timeout);
	void markArrow(int from, int to, QColor color, int timeout);

public slots:
	void showContextMenu(const QPoint& pos);
	void flip();
	void timeout();

signals:
	void moveEntered(ChessMove&);

protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;

private:
	QVector<BoardWindowMarkupSquare> squareMarkup;
	QVector<BoardWindowMarkupArrow> arrowMarkup;
	ChessBoard currentBoard;
	bool whiteAtBottom;
	int squareSize;
	QRect boardRect;
	QRect borderRect;
	BOARDTHEMA theme;
	typePiece dragPiece;
	QRect dragPieceRect;
	bool autoQueen;
	int dragFromSquare;
	void drawBorder(QPaintEvent* event, QPainter& painter);
	void drawCoordinates(QPaintEvent* event, QPainter& painter);
	void drawBoard(QPaintEvent* event, QPainter& painter);
	void drawPieces(QPaintEvent* event, QPainter& painter);
	void drawMarkupSquare(QPaintEvent* event, QPainter& painter);
	void drawMarkupArrow(QPaintEvent* event, QPainter& painter);
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