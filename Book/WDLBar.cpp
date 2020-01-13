#include "WDLBar.h"
#include <QImage>
#include <QPainter>

WDLBar::WDLBar(int w, int d, int l)
	: QBrush()
{
	sizeW = w;
	sizeD = d;
	sizeL = l;
	image = new QImage(100, 20, QImage::Format_RGB32);
	QPainter* paint = new QPainter(image);
	paint->setPen(QColor(Qt::black));
//	setStyle(Qt::SolidPattern);
	paint->drawLine(8, 9, 90, 9);
	paint->drawLine(8, 10, 90, 10);
	setTextureImage(*image);
}
