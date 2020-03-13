#include "../Common/BoardOption.h"
#include <QColorDialog>
#include <QSettings>
#include <QDialogButtonBox>
#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>

BoardOption::BoardOption(QWidget* parent)
	:QDialog(parent)
{
	QVBoxLayout* vbox;
	QHBoxLayout* hbox;
	QPushButton* button;
	QLabel* label;

	lightSquareColor = QColor(255, 255, 191, 255);

	vbox = new QVBoxLayout;

	hbox = new QHBoxLayout;
	label = new QLabel(tr("Light square color:"));
//	enginePath = new QLineEdit;
	button = new QPushButton("...");
	connect(button, SIGNAL(clicked()), this, SLOT(slotLightSquare()));
	hbox->addWidget(label);
//	hbox->addWidget(enginePath);
	hbox->addWidget(button);
	vbox->addLayout(hbox);

	QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok, this);
	vbox->addWidget(buttons);

	setLayout(vbox);

	connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
}

void BoardOption::slotLightSquare()
{

}