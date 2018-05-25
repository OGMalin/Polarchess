#include "BoardWindow.h"
#include <QPaintEvent>
#include <QPainter>
#include "../Common/defs.h"

BoardWindow::BoardWindow(QWidget* parent)
	:QWidget(parent)
{
	whiteAtBottom = true;
	setBoardTheme();
	setMinimumSize(200, 200);
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