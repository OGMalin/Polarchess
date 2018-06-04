#include "BoardWindow.h"
#include <QPaintEvent>
#include <QResizeEvent>
#include <QPainter>
#include "../Common/defs.h"

BoardWindow::BoardWindow(QWidget* parent)
	:QWidget(parent)
{
	whiteAtBottom = true;
	dragPiece = EMPTY;
	autoQueen = true;
	currentBoard.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	setBoardTheme();
	setMinimumSize(200, 200);
	readImgPieces();
}

void BoardWindow::paintEvent(QPaintEvent* event)
{
	calculateBoard();

	if (!event->rect().intersects(borderRect))
		return;

	QPainter painter(this);
	drawBorder(event, painter);
	drawBoard(event, painter);
	drawCoordinates(event, painter);
	painter.setRenderHint(QPainter::SmoothPixmapTransform);
	drawPieces(event, painter);
	drawDragPiece(event, painter);
}

void BoardWindow::setBoardTheme()
{
	theme.lightBrush.setColor(QColor(255,255,191,255));
	theme.lightBrush.setStyle(Qt::SolidPattern);
	theme.darkBrush.setColor(QColor(127, 127, 63, 255));
	theme.darkBrush.setStyle(Qt::SolidPattern);
	theme.borderBrush.setColor(QColor(200, 200, 63, 255));
	theme.borderBrush.setStyle(Qt::SolidPattern);
	theme.coordinateFont.setBold(true);
	theme.coordinateFontColor = QColor("white");
}

void BoardWindow::drawBorder(QPaintEvent* event, QPainter& painter)
{
	painter.drawRect(borderRect);
	painter.fillRect(borderRect.x() + 1, borderRect.y() + 1, borderRect.width()-2, borderRect.height()-2, theme.borderBrush);
	painter.drawRect(boardRect.x() - 1, boardRect.y() -1, boardRect.width()+2, boardRect.height()+2);
}

void BoardWindow::drawCoordinates(QPaintEvent* event, QPainter& painter)
{
	QRect sqF,sqR;
	QString f,r;
	int i;
	theme.coordinateFont.setPixelSize(squareSize / 4);
	painter.setFont(theme.coordinateFont);
	painter.setPen(theme.coordinateFontColor);
	if (whiteAtBottom)
	{
		f = "abcdefgh";
		r = "87654321";
	}
	else
	{
		f = "hgfedcba";
		r = "12345678";
	}
	sqF.setX(boardRect.x());
	sqF.setY(boardRect.y() + boardRect.height() + 1);
	sqF.setHeight(squareSize / 2);
	sqF.setWidth(squareSize);
	sqR.setX(borderRect.x() + 1);
	sqR.setY(boardRect.y());
	sqR.setHeight(squareSize);
	sqR.setWidth(squareSize / 2);
	for (i = 0; i < 8; i++)
	{
		painter.drawText(sqF, f.at(i),QTextOption(Qt::AlignCenter));
		painter.drawText(sqR, r.at(i), QTextOption(Qt::AlignCenter));
		sqF.translate(squareSize, 0);
		sqR.translate(0, squareSize);
	}
}

void BoardWindow::drawBoard(QPaintEvent* event, QPainter& painter)
{

	int sq;
	QRect sqR;
	bool light = true;
	for (sq=0;sq<64;sq++)
	{
		sqR = squareRect(sq);
		if (event->rect().intersects(sqR))
			painter.fillRect(sqR, (SQUARECOLOR(SQUARE128(sq))==WHITE) ? theme.lightBrush : theme.darkBrush);
	}
}

void BoardWindow::drawPieces(QPaintEvent* event, QPainter& painter)
{
	typePiece p;
	int sq;
	for (sq = 0; sq < 64; sq++)
	{
		p = currentBoard.board[SQUARE128(sq)];
		if (p != EMPTY) 
		{
			if ((dragPiece==EMPTY) || (dragFromSquare!=sq))
				painter.drawPixmap(squareRect(sq), imgPieces[p - 1]);
		}
	}
}

void BoardWindow::drawSquare(QPaintEvent* event, QPainter& painter, int sq, bool clear)
{
	QRect sqR = squareRect(sq);
	painter.fillRect(sqR, (SQUARECOLOR(SQUARE128(sq)) == WHITE) ? theme.lightBrush : theme.darkBrush);
	if (!clear)
	{
		typePiece p = currentBoard.board[SQUARE128(sq)];
		if (p != EMPTY)
			painter.drawPixmap(sqR, imgPieces[p - 1]);
	}
}

void BoardWindow::drawDragPiece(QPaintEvent* event, QPainter& painter)
{
	if (dragPiece == EMPTY)
		return;
	if (event->rect().intersects(dragPieceRect))
	{
		painter.drawPixmap(dragPieceRect, imgPieces[dragPiece-1]);
	}

}

void BoardWindow::calculateBoard()
{
	int size = qMin(width(), height());
	squareSize = (((size - 4) / 9)/2)*2;
	int w = squareSize * 9 + 4;

	borderRect.setX((width() - w) / 2);
	borderRect.setY((height() - w) / 2);

	borderRect.setWidth(squareSize * 9 + 4);
	borderRect.setHeight(borderRect.width());

	boardRect.setX(borderRect.x() + squareSize / 2 + 2);
	boardRect.setY(borderRect.y() + squareSize / 2 + 2);
	boardRect.setWidth(squareSize * 8);
	boardRect.setHeight(boardRect.width());
}

QRect BoardWindow::squareRect(int sq)
{
	QRect rect;
	int file = FILE(SQUARE128(sq));
	int rank = RANK(SQUARE128(sq));
	if (whiteAtBottom)
		rank = abs(rank - 7);
	else
		file = abs(file - 7);
	rect.setX(boardRect.x() + file * squareSize);
	rect.setY(boardRect.y() + rank * squareSize);
	rect.setWidth(squareSize);
	rect.setHeight(squareSize);
	return rect;
}

void BoardWindow::flip()
{
	flip(whiteAtBottom ? true : false);
}

void BoardWindow::flip(bool inv)
{
	whiteAtBottom = inv ? false : true;
	repaint();
}

void BoardWindow::readImgPieces()
{
	int i;
	QPixmap allPieces;
	allPieces.load(":/images/pieces.png");
	int w = allPieces.width() / 12;
	for (i = 0; i < 12; i++)
	{
		imgPieces[i] = allPieces.copy(i*w, 0, w, w);
	}
}

void BoardWindow::mousePressEvent(QMouseEvent* event)
{
	if (event->buttons() == Qt::LeftButton)
	{
		int sq = findSquare(event->pos());
		if (sq >= 0)
		{
			typePiece p = currentBoard.board[SQUARE128(sq)];
			if (p != EMPTY)
			{
				dragPieceRect.setX(event->pos().x() - squareSize / 2);
				dragPieceRect.setY(event->pos().y() - squareSize / 2);
				dragPieceRect.setWidth(squareSize);
				dragPieceRect.setHeight(squareSize);

				dragPiece = p;
				dragFromSquare = sq;

				update(dragPieceRect);
				update(squareRect(sq));
				QWidget::mousePressEvent(event);
				return;
			}
		}
	}
	dragPiece = EMPTY;
	QWidget::mousePressEvent(event);
}

void BoardWindow::mouseMoveEvent(QMouseEvent* event)
{
	if ((event->buttons() & Qt::LeftButton) && (dragPiece!=EMPTY))
	{
		QRect rect = dragPieceRect;
		dragPieceRect.setX(event->pos().x() - squareSize / 2);
		dragPieceRect.setY(event->pos().y() - squareSize / 2);
		dragPieceRect.setWidth(squareSize);
		dragPieceRect.setHeight(squareSize);
		update(rect);
		update(dragPieceRect);
	}

	QWidget::mouseMoveEvent(event);
}

void BoardWindow::mouseReleaseEvent(QMouseEvent* event)
{
	if (!(event->buttons() & Qt::LeftButton))
	{
		int sq = findSquare(event->pos());
		if (dragPiece != EMPTY)
		{
			if (sq >= 0)
			{
				ChessMove m;
				m.fromSquare = SQUARE128(dragFromSquare);
				m.toSquare = SQUARE128(sq);
				// Promotion
				if (((dragPiece == whitepawn) && (sq>55)) || ((dragPiece == blackpawn) && (sq<8)))
				{
					if (autoQueen)
					{
						m.promotePiece = (dragPiece == whitepawn) ? whitequeen : blackqueen;
						m.moveType |= PROMOTE;
					}
					else
					{
						// Make a dialog to get the promoting piece
					}
				}
				if (currentBoard.isLegal(m))
				{
					// Trix to fill out the rest of the move (castle, ep etc.);
					m=currentBoard.getMoveFromText(currentBoard.makeMoveText(m,UCI));
					
					currentBoard.doMove(m,false);
					currentBoard.board[SQUARE128(dragFromSquare)] = EMPTY;
					if (m.moveType&PROMOTE)
						currentBoard.board[SQUARE128(sq)] = m.promotePiece;
					else
						currentBoard.board[SQUARE128(sq)] = dragPiece;
					emit moveEntered(m);
				}
			}
		}
		dragPiece = EMPTY;
		update();
//		update(dragPieceRect);
//		update(squareRect(dragFromSquare));
//		update(squareRect(sq));
		QWidget::mouseReleaseEvent(event);
	}
}

int BoardWindow::findSquare(const QPoint& point)
{
	if (boardRect.contains(point.x(), point.y()))
	{
		int sq;
		for (sq = 0; sq < 64; sq++)
		{
			if (squareRect(sq).contains(point.x(), point.y()))
				return sq;
		}
	}
	return -1;
}

void BoardWindow::newGame()
{
	whiteAtBottom = true;
	dragPiece = EMPTY;
	currentBoard.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	update();
}

void BoardWindow::setPosition(QString& fen)
{
	dragPiece = EMPTY;
	currentBoard.setFen(fen.toLatin1());
	update();
}