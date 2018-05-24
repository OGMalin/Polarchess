#include "BoardWindow.h"
#include <QPainter>
#include "../Common/defs.h"

BoardWindow::BoardWindow(QWidget* parent)
	:QWidget(parent)
{
	setBoardStyle();
}

void BoardWindow::paintEvent(QPaintEvent* event)
{
	int i, j, x, y;
	int size = qMin(width(), height());
	squareSize = (size-4)/9;
	int boardSize = (squareSize * 9) + 2;
	int origoX = (width() - boardSize) / 2;
	int origoY = (height() - boardSize) / 2;

	QPainter painter(this);

	painter.drawRect(origoX, origoY, boardSize, boardSize);
	painter.fillRect(origoX+1, origoY+1, squareSize * 9+2, squareSize * 9+2, borderBrush);
	painter.drawRect(origoX + (squareSize / 2) + 1, origoY + (squareSize / 2) + 1, squareSize * 8 + 2, squareSize * 8 + 2);
	bool light = true;
	for (i = 0; i < 8; i++)
	{
		y = origoY+(squareSize / 2)+2 + squareSize * i;
		for (j = 0; j < 8; j++)
		{
			x = origoX + (squareSize / 2) + 2 + squareSize * j;
			painter.fillRect(x, y, squareSize, squareSize,light?lightBrush:darkBrush);
			light = light ? false : true;;
		}
		light = light ? false : true;;
	}
}

void BoardWindow::setBoardStyle()
{
	lightBrush.setColor(QColor(255,255,191,255));
	lightBrush.setStyle(Qt::SolidPattern);
	darkBrush.setColor(QColor(127, 127, 63, 255));
	darkBrush.setStyle(Qt::SolidPattern);
	borderBrush.setColor(QColor(63, 63,31, 255));
	borderBrush.setStyle(Qt::SolidPattern);
}