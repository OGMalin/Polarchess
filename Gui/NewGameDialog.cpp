#include "NewGameDialog.h"
#include <QtWidgets>
#include <QGridLayout>
#include <QPushButton>

NewGameDialog::NewGameDialog(QWidget *parent)
	: QDialog(parent)
{
	setWindowTitle(tr("New game"));
	resize(600, 400);
	setSizeGripEnabled(true);
/*
	QString engineName;
	QString player;
	int gameType;
	int moves;
	int startTime;
	int startTimeInc;
	int suddenDeathTime;
	bool rated;
	// 0=white, 1=black, 2=random
	int color;
*/
	QGridLayout* grid = new QGridLayout;
	QHBoxLayout* hbox;
	QVBoxLayout* vbox;
	QGroupBox* group;
	QPushButton* button;
	QComboBox* combo;

	group = new QGroupBox(tr("Your name"));
	playername = new QLineEdit(group);
	playername->setReadOnly(true);
	hbox = new QHBoxLayout;
	hbox->addWidget(playername);
	button = new QPushButton(tr("Change player"));
	hbox->addWidget(button);
	hbox->addStretch(1);
	group->setLayout(hbox);
	grid->addWidget(group, 0, 0, Qt::AlignLeft);
	connect(button, SIGNAL(clicked()), this, SLOT(slotSelectPlayer()));

	group = new QGroupBox(tr("Opponent"));
	combo = new QComboBox;
	combo->addItems(getEnginePlayers());



	QPushButton* cancel = new QPushButton(tr("Cancel"), this);
	QPushButton* ok = new QPushButton(tr("Ok"), this);
	ok->setAutoDefault(true);

	grid->addWidget(cancel, 2, 0, Qt::AlignRight | Qt::AlignBottom);
	grid->addWidget(ok, 2, 1, Qt::AlignRight | Qt::AlignBottom);

	setLayout(grid);

	connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));
	connect(ok, SIGNAL(clicked(bool)), this, SLOT(slotOk(bool)));
}

NewGameDialog::~NewGameDialog()
{
}

void NewGameDialog::slotOk(bool)
{
	accept();
}

const NewGameSetting NewGameDialog::getSetting()
{
	return setting;

}

void NewGameDialog::setDefault(const NewGameSetting& newsetting)
{
	setting = newsetting;
	if (setting.player.isEmpty())
	{
		setting.player = getenv("USER");
		if (setting.player.isEmpty())
			setting.player = getenv("USERNAME");
		setting.moves = 40;
		setting.startTime = 90 * 60;
		setting.startTimeInc = 30;
		setting.suddenDeathTime = 0;
		setting.rated = false;
	}
	playername->setText(setting.player);
}

void NewGameDialog::slotSelectPlayer()
{
}

const QStringList NewGameDialog::slotSelectPlayer()
{
#ifdef _DEBUG
	QDir dir("../x64/Debug/personalities");
#else
	QDir dir("personalities");
#endif

	QStringList list;
	if (!dir.exists)
		return list;
	dir.setFilter(QDir::Files);
	dir.setSorting(QDir::IgnoreCase);
	return list;
}
