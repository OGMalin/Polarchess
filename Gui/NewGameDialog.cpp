#include "NewGameDialog.h"
#include "PlayerDialog.h"
#include <QGridLayout>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QTimeEdit>
#include <QLabel>
#include <QGroupBox>
#include <QDir>
#include <QMap>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QSettings>
#include <QFrame>
#include <QSpinBox>
#include <QSlider>
#include <QSpacerItem>

NewGameDialog::NewGameDialog(QWidget *parent)
	: QDialog(parent)
{
	setWindowTitle(tr("New game"));
	setSizeGripEnabled(true);
	QGridLayout* grid = new QGridLayout(this); //Main grid
	QHBoxLayout* hbox;
	QVBoxLayout* vbox;
	QGroupBox* group;
	QComboBox* combo;
	QPushButton* button1;
	QPushButton* button2;
	QLabel* label1;
	QLabel* label2;
	QLabel* label3;
	QLabel* label4;
	QLabel* label5;
	QDialogButtonBox* buttonBox;
	QFrame* frame;
	QSpacerItem* spacer;

	frame = new QFrame(this);

	group = new QGroupBox(tr("Your name"),frame);
	hbox = new QHBoxLayout(group);
	playername = new QLineEdit(group);
	playername->setReadOnly(true);
	hbox->addWidget(playername);
	button1 = new QPushButton("...", group);
	button1->setMaximumWidth(30);
	hbox->addWidget(button1);
	grid->addWidget(group, 0, 0);
	connect(button1, SIGNAL(clicked()), this, SLOT(slotSelectPlayer()));
//
	group = new QGroupBox(tr("Opponent"), frame);
	vbox = new QVBoxLayout(group);
	hbox = new QHBoxLayout(group);
	label1 = new QLabel(tr("Engine:"), group);
	enginename = new QLabel(group);
	hbox->addWidget(label1);
	hbox->addWidget(enginename);
	vbox->addLayout(hbox);
//
	// Komodo 13+
	hbox = new QHBoxLayout(group);
	label1 = new QLabel(tr("Personality:"),group);
	personality = new QComboBox(group);
	hbox->addWidget(label1);
	hbox->addWidget(personality);
	vbox->addLayout(hbox);
	hbox = new QHBoxLayout(group);
	label1 = new QLabel(tr("Skill:"), group);
	skill = new QSpinBox(group);
	hbox->addWidget(label1);
	hbox->addWidget(skill);
	vbox->addLayout(hbox);
	autoskill = new QCheckBox(tr("Auto skill"),group);
	vbox->addWidget(autoskill);

	// Generic UCI engine
	fullstrength = new QCheckBox(tr("Full strength"), group);
	vbox->addWidget(fullstrength);
	limitstrength = new QSlider(group);
	limitstrength->setOrientation(Qt::Horizontal);
	vbox->addWidget(limitstrength);
	connect(fullstrength, SIGNAL(clicked(bool)), this, SLOT(slotStrengthClicked(bool)));
	connect(limitstrength, SIGNAL(valueChanged(int)), this, SLOT(slotStrengthChanged(int)));

	hbox = new QHBoxLayout(group);
	label1 = new QLabel(tr("Rating:"), group);
	computerelo = new QLabel(group);
	hbox->addWidget(label1);
	hbox->addWidget(computerelo);
	vbox->addLayout(hbox);
	grid->addWidget(group, 1, 0, 2, 1);

	group = new QGroupBox(tr("Timecontrol"), frame);
	vbox = new QVBoxLayout(group);

	hbox = new QHBoxLayout(group);
	label1 = new QLabel(tr("Predefined timecontrols:"), group);
	combo = new QComboBox(group);
	combo->addItem(tr(""));
	combo->addItem(tr("90 min. + 30 sec."));
	combo->addItem(tr("30 min."));
	combo->addItem(tr("15 min. + 10 sec."));
	combo->addItem(tr("5 min."));
	combo->addItem(tr("3 min. + 2 sec."));
	combo->addItem(tr("1 min. + 2 sec."));
	hbox->addWidget(label1);
	hbox->addWidget(combo);
	vbox->addLayout(hbox);
	connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(slotPreTime(int)));

	hbox = new QHBoxLayout(group);
	label1 = new QLabel(tr("Start time:"), group);
	starttime = new QTimeEdit(group);
	starttime->setDisplayFormat("HH::mm:ss");
	starttime->setMinimumTime(QTime(0, 0, 30));
	hbox->addWidget(label1);
	hbox->addWidget(starttime);
	vbox->addLayout(hbox);
	connect(starttime, SIGNAL(timeChanged(const QTime&)), this, SLOT(slotTimeChanged(const QTime&)));

	hbox = new QHBoxLayout(group);
	label1 = new QLabel(tr("Increment:"), group);
	starttimeinc = new QTimeEdit(group);
	starttimeinc->setDisplayFormat("mm:ss");
	starttimeinc->setMaximumTime(QTime(0, 59, 59));
	starttimeinc->setCurrentSection(QDateTimeEdit::MinuteSection);
	hbox->addWidget(label1);
	hbox->addWidget(starttimeinc);
	vbox->addLayout(hbox);
	connect(starttimeinc, SIGNAL(timeChanged(const QTime&)), this, SLOT(slotTimeChanged(const QTime&)));

	moves = new QCheckBox(tr("New time at 40 moves"), group);
	vbox->addWidget(moves);

	hbox = new QHBoxLayout(group);
	label1 = new QLabel(tr("Sudden death:"), group);
	suddendeath = new QTimeEdit(group);
	suddendeath->setDisplayFormat("HH::mm:ss");
	hbox->addWidget(label1);
	hbox->addWidget(suddendeath);
	vbox->addLayout(hbox);
	connect(moves, SIGNAL(clicked(bool)), suddendeath, SLOT(setEnabled(bool)));
	connect(suddendeath, SIGNAL(timeChanged(const QTime&)), this, SLOT(slotTimeChanged(const QTime&)));

	hbox = new QHBoxLayout(group);
	label1 = new QLabel(tr("Rating categori:"), group);
	gametype = new QLabel(group);
	hbox->addWidget(label1);
	hbox->addWidget(gametype);
	vbox->addLayout(hbox);

	grid->addWidget(group, 0, 1, 2, 1);

	group = new QGroupBox(frame);
	vbox = new QVBoxLayout(group);
	hbox = new QHBoxLayout(group);
	rated = new QCheckBox(tr("Rated game"), group);
	label1 = new QLabel(tr("Color:"), group);
	color = new QComboBox(group);
	color->addItem(tr("White"));
	color->addItem(tr("Black"));
	color->addItem(tr("Random"));
	hbox->addWidget(rated);
	hbox->addWidget(label1);
	hbox->addWidget(color);
	vbox->addLayout(hbox);
	hbox = new QHBoxLayout(group);
	label1 = new QLabel(tr("Startposition:"), group);
	startposition = new QLabel(tr("Standard"), group);
	button1 = new QPushButton("...", group);
	button1->setMaximumWidth(30);
	hbox->addWidget(label1);
	hbox->addWidget(startposition);
	hbox->addWidget(button1);
	vbox->addLayout(hbox);
	connect(button1, SIGNAL(clicked()), this, SLOT(slotStartposition()));

	grid->addWidget(group, 2, 1, 1, 1);

	buttonBox = new QDialogButtonBox(this);
	buttonBox->setOrientation(Qt::Horizontal);
	buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
	grid->addWidget(buttonBox, 3, 0, 1, 2);
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(slotOk()));
}

NewGameDialog::~NewGameDialog()
{
}

void NewGameDialog::slotOk()
{
	//setting.color = color->currentIndex();
	//setting.computer = computer->currentText();
	//setting.player = playername->text();
	//setting.startTime = -starttime->time().secsTo(QTime(0, 0, 0));
	//setting.startTimeInc = -starttimeinc->time().secsTo(QTime(0, 0, 0));
	//if (moves->isChecked())
	//	setting.suddenDeathTime = -suddendeath->time().secsTo(QTime(0, 0, 0));
	//else
	//	setting.suddenDeathTime = 0;
	//setting.rated = rated->isChecked();
	accept();
}

void NewGameDialog::slotStrengthChanged(int value)
{
	char sz[16];
	computerelo->setText(itoa(value, sz, 10));
}

void NewGameDialog::slotStrengthClicked(bool value)
{
	char sz[16];
	limitstrength->setDisabled(value);
	if (value)
		computerelo->setText(itoa(3500, sz, 10));
	else
		computerelo->setText(itoa(limitstrength->value(), sz, 10));
}

void NewGameDialog::slotSelectPlayer()
{
	PlayerDialog dialog(this, playername->text());
	if (dialog.exec() == QDialog::Rejected)
		return;
	playername->setText(dialog.text());
}

void NewGameDialog::slotStartposition()
{
}

void NewGameDialog::slotTimeChanged(const QTime&)
{
	setGameType();
}

void NewGameDialog::slotPreTime(int index)
{
	switch (index)
	{
	case 1:
		starttime->setTime(QTime(1, 30, 0));
		starttimeinc->setTime(QTime(0, 0, 30));
		suddendeath->setTime(QTime(0, 0, 0));
		moves->setChecked(false);
		suddendeath->setEnabled(false);
		break;
	case 2:
		starttime->setTime(QTime(0, 30, 0));
		starttimeinc->setTime(QTime(0, 0, 0));
		suddendeath->setTime(QTime(0, 0, 0));
		moves->setChecked(false);
		suddendeath->setEnabled(false);
		break;
	case 3:
		starttime->setTime(QTime(0, 15, 0));
		starttimeinc->setTime(QTime(0, 0, 10));
		suddendeath->setTime(QTime(0, 0, 0));
		moves->setChecked(false);
		suddendeath->setEnabled(false);
		break;
	case 4:
		starttime->setTime(QTime(0, 5, 0));
		starttimeinc->setTime(QTime(0, 0, 0));
		suddendeath->setTime(QTime(0, 0, 0));
		moves->setChecked(false);
		suddendeath->setEnabled(false);
		break;
	case 5:
		starttime->setTime(QTime(0, 3, 0));
		starttimeinc->setTime(QTime(0, 0, 2));
		suddendeath->setTime(QTime(0, 0, 0));
		moves->setChecked(false);
		suddendeath->setEnabled(false);
		break;
	case 6:
		starttime->setTime(QTime(0, 1, 0));
		starttimeinc->setTime(QTime(0, 0, 2));
		suddendeath->setTime(QTime(0, 0, 0));
		moves->setChecked(false);
		suddendeath->setEnabled(false);
		break;
	}
}

void NewGameDialog::setGameType()
{
	QTime st = starttime->time();
	QTime sti = starttimeinc->time();
	QTime sud = suddendeath->time();

	int t = st.hour() * 3600 + st.minute() * 60 + st.second();
	// Bullet count to 40 moves for increment
	int tb = t + sti.hour() * 3600 * 40 + sti.minute() * 60 * 40 + sti.second() * 40;
	t += sti.hour() * 3600 * 60 + sti.minute() * 60 * 60 + sti.second() * 60;
	if (tb < 180)
	{
		gametype->setText(tr("Bullet chess"));
		setting.gameType = BULLET;
	}
	else if (t <= 600)
	{
		gametype->setText(tr("Blitz chess"));
		setting.gameType = BLITZ;
	}
	else if (t < 3600)
	{
		gametype->setText(tr("Rapid chess"));
		setting.gameType = RAPID;
	}
	else
	{
		gametype->setText(tr("Classical chess"));
		setting.gameType = CLASSICAL;
	}
}

const NewGameSetting NewGameDialog::getSetting()
{
	return setting;

}

void NewGameDialog::setDefault(NewGameSetting& newsetting, Engine* eng)
{
	char sz[16];
	int i;
	//int h, m, s;
	setting = newsetting;
	//engines = eng;
	playername->setText(setting.player);
	enginename->setText(eng->name);

	if (setting.rated)
		rated->setChecked(true);
	if (setting.suddenDeathTime > 0)
	{
		moves->setChecked(true);
		suddendeath->setTime(QTime(0, 0, 0).addSecs(setting.suddenDeathTime));
	}
	else
	{
		moves->setChecked(false);
		suddendeath->setEnabled(false);
		suddendeath->setTime(QTime(0, 0, 0));
	}

	starttime->setTime(QTime(0, 0, 0).addSecs(setting.startTime));
	starttimeinc->setTime(QTime(0, 0, 0).addSecs(setting.startTimeInc));

	color->setCurrentIndex(setting.color);
	setGameType();

	computerelo->setText(itoa(setting.engineelo,sz,10));

	if (eng->personality.name == "Personality")
	{
		for (i = 0; i < eng->personality.combo.var.size(); i++)
			personality->addItem(eng->personality.combo.var[i]);
		personality->setCurrentText(eng->personality.combo.default);
	}
	else
	{
		personality->setVisible(false);
	}
	if (eng->skill.name == "Skill")
	{
		skill->setMinimum(eng->skill.spin.min);
		skill->setMaximum(eng->skill.spin.max);
		skill->setValue(setting.engineskill);
	}
	else
	{
		skill->setVisible(false);
	}

	if (eng->autoskill.name == "Auto Skill")
		autoskill->setChecked(setting.aoutoskill);
	else
		autoskill->setVisible(false);

	if (eng->limitstrength.name == "UCI_LimitStrength")
		fullstrength->setChecked(setting.fullstrength);
	else
		fullstrength->setVisible(false);

	if (eng->elo.name == "UCI_Elo")
	{
		limitstrength->setMinimum(eng->elo.spin.min);
		limitstrength->setMaximum(eng->elo.spin.max);
		limitstrength->setValue(setting.engineelo);
	}
	else
	{
		limitstrength->setVisible(false);
	}

	resize(minimumSizeHint());
}

