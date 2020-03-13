#pragma once

#include <QDialog>
#include <QColor>

class BoardOption :public QDialog
{
public slots:
	void slotLightSquare();

public:
	BoardOption(QWidget* parent);

private:
	QColor lightSquareColor;
};