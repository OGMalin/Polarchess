#include "../Common/BoardWindow.h"
#include <QPaintEvent>
#include <QResizeEvent>
#include <QPainter>
#include <QMenu>
#include <QTimer>
#include "../Common/defs.h"

BoardWindow::BoardWindow(QWidget* parent)
	:QWidget(parent)
{
	dragFromSquare = 0;
	squareSize = 50;
	whiteAtBottom = true;
	dragPiece = EMPTY;
	autoQueen = true;
	currentBoard.setStartposition();
	setDefaultBoardTheme();
	setMinimumSize(200, 200);
	readImgPieces();
	QTimer* timer = new QTimer();
	connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
	timer->start(1000);
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));
}

void BoardWindow::paintEvent(QPaintEvent* event)
{
	calculateBoard();

	if (!event->rect().intersects(borderRect))
		return;

	QPainter painter(this);
	drawBorder(event, painter);
	drawBoard(event, painter);
	drawMarkupSquare(event, painter);
	drawMarkupArrow(event, painter);
	drawCoordinates(event, painter);
	painter.setRenderHint(QPainter::SmoothPixmapTransform);
	drawPieces(event, painter);
	drawDragPiece(event, painter);
}

void BoardWindow::setDefaultBoardTheme()
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

	int sq,i;
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

void BoardWindow::drawMarkupSquare(QPaintEvent* event, QPainter& painter)
{
	QRect sqR;
	int i;
	painter.setOpacity(0.3);
	for (i = 0; i < squareMarkup.size(); i++)
	{
		sqR = squareRect(squareMarkup[i].square);
		if (event->rect().intersects(sqR))
			painter.fillRect(sqR, squareMarkup[i].color);
	}
	painter.setOpacity(1.0);
}

void BoardWindow::drawMarkupArrow(QPaintEvent* event, QPainter& painter)
{
	QRect sqR;
	QPoint from, to;
	QPen pen;
	int i,w;
	painter.setOpacity(0.3);
	pen = painter.pen();
	pen.setStyle(Qt::SolidLine);
	for (i = 0; i < arrowMarkup.size(); i++)
	{
		pen.setColor(arrowMarkup[i].color);
		sqR = squareRect(arrowMarkup[i].fromSquare);
		from.setX(sqR.x()+sqR.width()/2);
		from.setY(sqR.y() + sqR.width() / 2);
		sqR = squareRect(arrowMarkup[i].toSquare);
		to.setX(sqR.x() + sqR.width() / 2);
		to.setY(sqR.y() + sqR.width() / 2);
		w = sqR.width() / 10;
		if (w < 2)
			w = 2;
		pen.setWidth(w);
		pen.setCapStyle(Qt::RoundCap);
		painter.setPen(pen);
		painter.drawLine(from, to);
	}
	painter.setOpacity(1.0);
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
	squareMarkup.clear();
	arrowMarkup.clear();
	update();
}

void BoardWindow::setPosition(const QString& fen)
{
	dragPiece = EMPTY;
	currentBoard.setFen(fen.toLatin1());
	squareMarkup.clear();
	arrowMarkup.clear();
	update();
}

void BoardWindow::setPosition(const ChessBoard& cb)
{
	dragPiece = EMPTY;
	currentBoard = cb;
	squareMarkup.clear();
	arrowMarkup.clear();
	update();
}

void BoardWindow::setPiece(int sq, int piece)
{
	dragPiece = EMPTY;
	currentBoard.board[SQUARE128(sq)] = piece;
	squareMarkup.clear();
	arrowMarkup.clear();
	update();
}

void BoardWindow::showContextMenu(const QPoint& pos)
{
	QMenu* contextMenu = new QMenu(this);
	contextMenu->addAction(tr("Flip"), this, SLOT(flip()));
	contextMenu->addAction(tr("Board options"), this, SLOT(setBoardOptions()));
	contextMenu->exec(mapToGlobal(pos));
}

void BoardWindow::markSquare(int sq, QColor color, int t)
{
	BoardWindowMarkupSquare mark;
	mark.square = sq;
	mark.color = color;
	mark.timeout = t;
	squareMarkup.push_back(mark);
	update();
}

void BoardWindow::markArrow(int from, int to, QColor color, int t)
{
	BoardWindowMarkupArrow mark;
	mark.fromSquare = from;
	mark.toSquare = to;
	mark.color = color;
	mark.timeout = t;
	arrowMarkup.push_back(mark);
	update();
}

void BoardWindow::timeout()
{
	bool updateNow = false;
	int i;
	for (i = 0; i < squareMarkup.size(); i++)
	{
		if (squareMarkup[i].timeout > 0)
		{
			--squareMarkup[i].timeout;
			if (squareMarkup[i].timeout == 0)
			{
				squareMarkup.remove(i);
				--i;
				updateNow = true;
			}
		}
	}
	for (i = 0; i < arrowMarkup.size(); i++)
	{
		if (arrowMarkup[i].timeout > 0)
		{
			--arrowMarkup[i].timeout;
			if (arrowMarkup[i].timeout == 0)
			{
				arrowMarkup.remove(i);
				--i;
				updateNow = true;
			}
		}
	}
	if (updateNow)
		update();
}

void BoardWindow::setBoardOptions()
{
	BoardOption dialog(this, theme);
	if (dialog.exec() == QDialog::Accepted)
	{
		theme=dialog.getThema();
		update();
	}
}

void BoardWindow::setTheme(BOARDTHEMA& t)
{
	theme = t;
	update();
}

BOARDTHEMA BoardWindow::getTheme()
{
	return theme;
}