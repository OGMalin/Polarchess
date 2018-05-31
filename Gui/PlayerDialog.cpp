#include "PlayerDialog.h"
#include <QListWidget>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

PlayerDialog::PlayerDialog(QWidget *parent)
	: QDialog(parent)
{
	setWindowTitle(tr("Player list"));
	setSizeGripEnabled(true);
	QGridLayout* grid = new QGridLayout;
	
	QLabel* label;
	QPushButton* button1;
	QPushButton* button2;
	QHBoxLayout* hbox;

	label = new QLabel;
	label->setText(tr("Players"));
	playerlist = new QListWidget;
	grid->addWidget(label, 0, 0, Qt::AlignLeft);
	grid->addWidget(playerlist, 1, 0, 2, 1, Qt::AlignLeft);

	button1 = new QPushButton(tr("New player"));
	button2 = new QPushButton(tr("Remove player"));
	grid->addWidget(button1, 1, 1, Qt::AlignLeft);
	grid->addWidget(button2, 2, 1, Qt::AlignLeft);

	hbox = new QHBoxLayout;
	button1 = new QPushButton(tr("Cancel"));
	button2 = new QPushButton(tr("Ok"));
	hbox->addWidget(button2);
	hbox->addWidget(button1);
	grid->addLayout(hbox, 3, 1, Qt::AlignRight | Qt::AlignBottom);
	connect(button1, SIGNAL(clicked()), this, SLOT(reject()));
	connect(button2, SIGNAL(clicked(bool)), this, SLOT(slotOk(bool)));

	setLayout(grid);

	resize(minimumSizeHint());
}

PlayerDialog::~PlayerDialog()
{
}

void PlayerDialog::slotOk(bool)
{
	accept();
}