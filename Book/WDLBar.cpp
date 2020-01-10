#include "WDLBar.h"
#include <QImage>

WDLBar::WDLBar(int w, int d, int l)
	: QBrush()
{
	sizeW = w;
	sizeD = d;
	sizeL = l;
	image = new QImage(100, 20, QImage::Format_RGB32);
	setStyle(Qt::SolidPattern);
	for (int x = 0; x < 100; x++)
	{
		image->setPixel(x, 8, 0xff555555);
		image->setPixel(x, 9, 0xff555555);
		image->setPixel(x, 10, 0xff555555);
		image->setPixel(x, 11, 0xff555555);
		image->setPixel(x, 12, 0xff555555);
		image->setPixel(x, 13, 0xff555555);
	}
	setTextureImage(*image);
}

