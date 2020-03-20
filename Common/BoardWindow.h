#pragma once

#include <QWidget>
#include <QPixmap>
#include <QPoint>
#include <QColor>
#include "../Common/ChessBoard.h"
#include "../Common/ChessMove.h"
#include "../Common/BoardOption.h"

struct BoardWindowMarkupArrow
{
	int fromSquare=0;
	int toSquare=0;
	QColor color;
	int timeout=0;
};

struct BoardWindowMarkupSquare
{
	int square=0;
	QColor color;
	int timeout=0;
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
	void setTheme(BOARDTHEMA& t);
	BOARDTHEMA getTheme();

public slots:
	void showContextMenu(const QPoint& pos);
	void flip();
	void timeout();
	void setBoardOptions();

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
	void setDefaultBoardTheme();
	void calculateBoard();
	QRect squareRect(int sq);

	// Handling pieces
	QPixmap imgPieces[12];
	void readImgPieces();
};