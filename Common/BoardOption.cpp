#include "../Common/BoardOption.h"
#include <QColorDialog>
#include <QSettings>
#include <QDialogButtonBox>
#include <QBoxLayout>
#include <QLabel>

BoardOption::BoardOption(QWidget* parent, BOARDTHEMA& thema)
	:QDialog(parent)
{
	QVBoxLayout* vbox;
	QHBoxLayout* hbox;
	QLabel* label;
	QString qs;
	boardthema = thema;

	vbox = new QVBoxLayout;

	hbox = new QHBoxLayout;
	label = new QLabel(tr("Light square color:"));
	lightColor = new QPushButton;
	qs = "background-color: ";
	qs += thema.lightBrush.color().name();
	qs += ";";
	lightColor->setStyleSheet(qs);
	connect(lightColor, SIGNAL(clicked()), this, SLOT(slotLightColor()));
	hbox->addWidget(label);
	hbox->addWidget(lightColor);
	vbox->addLayout(hbox);
	hbox = new QHBoxLayout;
	label = new QLabel(tr("Dark square color:"));
	darkColor = new QPushButton;
	qs = "background-color: ";
	qs += thema.darkBrush.color().name();
	qs += ";";
	darkColor->setStyleSheet(qs);
	darkColor->setStyleSheet("background-color: rgb(243,65,87);");
	connect(darkColor, SIGNAL(clicked()), this, SLOT(slotDarkColor()));
	hbox->addWidget(label);
	hbox->addWidget(darkColor);
	vbox->addLayout(hbox);
	hbox = new QHBoxLayout;
	label = new QLabel(tr("Border color:"));
	borderColor = new QPushButton;
	qs = "background-color: ";
	qs += thema.borderBrush.color().name();
	qs += ";";
	borderColor->setStyleSheet(qs);
	connect(borderColor, SIGNAL(clicked()), this, SLOT(slotBorderColor()));
	hbox->addWidget(label);
	hbox->addWidget(borderColor);
	vbox->addLayout(hbox);
	hbox = new QHBoxLayout;
	label = new QLabel(tr("Coordnate color:"));
	coordinateColor = new QPushButton;
	qs = "background-color: ";
	qs += thema.coordinateFontColor.name();
	qs += ";";
	coordinateColor->setStyleSheet(qs);
	connect(coordinateColor, SIGNAL(clicked()), this, SLOT(slotCoordinateColor()));
	hbox->addWidget(label);
	hbox->addWidget(coordinateColor);
	vbox->addLayout(hbox);

	QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok, this);
	vbox->addWidget(buttons);

	setLayout(vbox);

	connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
}

void BoardOption::slotLightColor()
{
	QColor c = QColorDialog::getColor(boardthema.lightBrush.color(), this, tr("Select color for light squares"));
	if (c.isValid())
	{
		boardthema.lightBrush.setColor(c);
		QString qs = "background-color: ";
		qs += boardthema.lightBrush.color().name();
		qs += ";";
		lightColor->setStyleSheet(qs);
	}
}

void BoardOption::slotDarkColor()
{
	QColor c = QColorDialog::getColor(boardthema.darkBrush.color(), this, tr("Select color for dark squares"));
	if (c.isValid())
	{
		boardthema.darkBrush.setColor(c);
		QString qs = "background-color: ";
		qs += boardthema.darkBrush.color().name();
		qs += ";";
		darkColor->setStyleSheet(qs);
	}
}

void BoardOption::slotBorderColor()
{
	QColor c = QColorDialog::getColor(boardthema.borderBrush.color(), this, tr("Select color for border"));
	if (c.isValid())
	{
		boardthema.borderBrush.setColor(c);
		QString qs = "background-color: ";
		qs += boardthema.borderBrush.color().name();
		qs += ";";
		borderColor->setStyleSheet(qs);
	}
}

void BoardOption::slotCoordinateColor()
{
	QColor c = QColorDialog::getColor(boardthema.coordinateFontColor, this, tr("Select color for coordinate font"));
	if (c.isValid())
	{
		boardthema.coordinateFontColor=c;
		QString qs = "background-color: ";
		qs += boardthema.coordinateFontColor.name();
		qs += ";";
		coordinateColor->setStyleSheet(qs);
	}
}

BOARDTHEMA BoardOption::getThema()
{
	return boardthema;
}