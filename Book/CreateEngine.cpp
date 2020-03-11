#include "CreateEngine.h"
#include <QBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QPushButton>

CreateEngine::CreateEngine(QWidget* parent)
	:QDialog(parent)
{
	QVBoxLayout* vbox;
	QHBoxLayout* hbox;
	QPushButton* button;
	QLabel* label;
	vbox = new QVBoxLayout;

	hbox = new QHBoxLayout;
	label = new QLabel(tr("Engine type:"));
	engineType = new QComboBox;
	engineType->addItem("Uci");
	engineType->addItem("XBoard");
	hbox->addWidget(label);
	hbox->addWidget(engineType);
	vbox->addLayout(hbox);

	hbox = new QHBoxLayout;
	label = new QLabel(tr("Engine path:"));
	enginePath = new QLineEdit;
	button = new QPushButton("...");
	hbox->addWidget(label);
	hbox->addWidget(enginePath);
	hbox->addWidget(button);
	vbox->addLayout(hbox);

	QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok, this);
	vbox->addWidget(buttons);

	setLayout(vbox);

	connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
}