#pragma once

#include <QBrush>
#include <QImage>

class WDLBar : public QBrush
{
public:
	WDLBar(int w, int d, int l);
	QImage* image;
private:
	int sizeW;
	int sizeD;
	int sizeL;
};
