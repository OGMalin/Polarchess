#include "PlayerDialog.h"
#include "Player.h"
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QInputDialog>

PlayerDialog::PlayerDialog(QWidget *parent, QString& current)
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
	playerlist = new QComboBox;
	playerlist->addItems(Player().getAll());
	playerlist->setCurrentText(current);
	grid->addWidget(label, 0, 0, Qt::AlignLeft);
	grid->addWidget(playerlist, 1, 0, 2, 1, Qt::AlignLeft);

	button1 = new QPushButton(tr("Add new player"));
	button2 = new QPushButton(tr("Remove player"));
	grid->addWidget(button1, 1, 1, Qt::AlignLeft);
	grid->addWidget(button2, 2, 1, Qt::AlignLeft);
	connect(button1, SIGNAL(clicked()), this, SLOT(slotAdd()));
	connect(button2, SIGNAL(clicked()), this, SLOT(slotRemove()));

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

const QString PlayerDialog::text()
{
	return playerlist->currentText();
}

void PlayerDialog::slotAdd()
{
	Player pl;
	pl.newPlayer(this);
	if (pl.name().isEmpty())
		return;

	if (playerlist->findText(pl.name(), Qt::MatchExactly) == -1)
		playerlist->addItem(pl.name());
	playerlist->setCurrentText(pl.name());
}

void PlayerDialog::slotRemove()
{
	int i = playerlist->currentIndex();
	if (i >= 0)
	{
		Player pl;
		pl.remove(playerlist->currentText());
		playerlist->removeItem(i);
	}
}
