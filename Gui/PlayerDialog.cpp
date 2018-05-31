#include "PlayerDialog.h"
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
	playerlist->addItems(getPlayers());
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

void PlayerDialog::slotAdd()
{
	QInputDialog input(this);
	input.setWindowTitle(tr("Add new player"));
	input.setCancelButtonText(tr("Cancel"));
	input.setInputMode(QInputDialog::TextInput);
	input.setLabelText(tr("New name"));
	if (input.exec() == QDialog::Rejected)
		return;
	QString player=input.textValue();
	if (player.isEmpty())
		return;

	if (playerlist->findText(player, Qt::MatchExactly) == -1)
		playerlist->addItem(player);
	playerlist->setCurrentText(player);
	//	QSettings settings;
//	settings.setValue("");
}

void PlayerDialog::slotRemove()
{
	int i = playerlist->currentIndex();
	if (i>=0)
		playerlist->removeItem(i);
}

const QStringList PlayerDialog::getPlayers()
{
	
	QSettings settings;
	settings.beginGroup("players");
	return settings.allKeys();
}
