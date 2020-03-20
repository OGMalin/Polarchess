#pragma once

#include <QDialog>
#include <QColor>
#include <QWidget>
#include <QPushButton>

struct BOARDTHEMA
{
	QBrush lightBrush;
	QBrush darkBrush;
	QBrush borderBrush;
	QFont  coordinateFont;
	QColor coordinateFontColor;
};

class BoardOption :public QDialog
{
	Q_OBJECT
public slots:
	void slotLightColor();
	void slotDarkColor();
	void slotBorderColor();
	void slotCoordinateColor();

public:
	BoardOption(QWidget* parent, BOARDTHEMA& theme);
	BOARDTHEMA getThema();

private:
	BOARDTHEMA boardthema;
	QPushButton* lightColor;
	QPushButton* darkColor;
	QPushButton* borderColor;
	QPushButton* coordinateColor;
};