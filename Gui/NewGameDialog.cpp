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
#include <QStandardPaths>

NewGameDialog::NewGameDialog(QWidget *parent, Engine* eng)
	: QDialog(parent)
{
	engine = eng;
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
	hbox = new QHBoxLayout();
	playername = new QLineEdit();
	playername->setReadOnly(true);
	hbox->addWidget(playername);
	button1 = new QPushButton("...");
	button1->setMaximumWidth(30);
	hbox->addWidget(button1);
	group->setLayout(hbox);
	grid->addWidget(group, 0, 0);
	connect(button1, SIGNAL(clicked()), this, SLOT(slotSelectPlayer()));
//
	group = new QGroupBox(tr("Opponent"), frame);
	vbox = new QVBoxLayout();
	hbox = new QHBoxLayout();
	label1 = new QLabel(tr("Engine:"));
	enginename = new QLabel();
	hbox->addWidget(label1);
	hbox->addWidget(enginename);
	vbox->addLayout(hbox);
//
	// Komodo 13+
	hbox = new QHBoxLayout();
	label1 = new QLabel(tr("Personality:"));
	personality = new QComboBox();
	hbox->addWidget(label1);
	hbox->addWidget(personality);
	vbox->addLayout(hbox);
	connect(personality, SIGNAL(currentIndexChanged(int)), this, SLOT(setRating()));
	hbox = new QHBoxLayout();
	label1 = new QLabel(tr("Skill:"));
	skill = new QSpinBox();
	hbox->addWidget(label1);
	hbox->addWidget(skill);
	connect(skill, SIGNAL(valueChanged(int)), this, SLOT(setRating()));
	vbox->addLayout(hbox);
	autoskill = new QCheckBox(tr("Auto skill"));
	vbox->addWidget(autoskill);
	connect(autoskill, SIGNAL(clicked(bool)), this, SLOT(setRating()));

	// Generic UCI engine
	fullstrength = new QCheckBox(tr("Full strength"));
	vbox->addWidget(fullstrength);
	limitstrength = new QSlider();
	limitstrength->setOrientation(Qt::Horizontal);
	vbox->addWidget(limitstrength);
	connect(fullstrength, SIGNAL(clicked(bool)), this, SLOT(slotStrengthClicked(bool)));
	connect(limitstrength, SIGNAL(valueChanged(int)), this, SLOT(slotStrengthChanged(int)));

	hbox = new QHBoxLayout();
	label1 = new QLabel(tr("Rating:"));
	computerelo = new QLabel();
	hbox->addWidget(label1);
	hbox->addWidget(computerelo);
	vbox->addLayout(hbox);
	group->setLayout(vbox);
	grid->addWidget(group, 1, 0, 2, 1);

	group = new QGroupBox(tr("Timecontrol"), frame);
	vbox = new QVBoxLayout();

	hbox = new QHBoxLayout();
	label1 = new QLabel(tr("Predefined timecontrols:"));
	combo = new QComboBox();
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

	hbox = new QHBoxLayout();
	label1 = new QLabel(tr("Start time:"));
	starttime = new QTimeEdit();
	starttime->setDisplayFormat("HH::mm:ss");
	starttime->setMinimumTime(QTime(0, 0, 30));
	hbox->addWidget(label1);
	hbox->addWidget(starttime);
	vbox->addLayout(hbox);
	connect(starttime, SIGNAL(timeChanged(const QTime&)), this, SLOT(slotTimeChanged(const QTime&)));

	hbox = new QHBoxLayout();
	label1 = new QLabel(tr("Increment:"));
	starttimeinc = new QTimeEdit();
	starttimeinc->setDisplayFormat("mm:ss");
	starttimeinc->setMaximumTime(QTime(0, 59, 59));
	starttimeinc->setCurrentSection(QDateTimeEdit::MinuteSection);
	hbox->addWidget(label1);
	hbox->addWidget(starttimeinc);
	vbox->addLayout(hbox);
	connect(starttimeinc, SIGNAL(timeChanged(const QTime&)), this, SLOT(slotTimeChanged(const QTime&)));

	moves = new QCheckBox(tr("New time at 40 moves"));
	vbox->addWidget(moves);

	hbox = new QHBoxLayout();
	label1 = new QLabel(tr("Sudden death:"));
	suddendeath = new QTimeEdit();
	suddendeath->setDisplayFormat("HH::mm:ss");
	hbox->addWidget(label1);
	hbox->addWidget(suddendeath);
	vbox->addLayout(hbox);
	connect(moves, SIGNAL(clicked(bool)), suddendeath, SLOT(setEnabled(bool)));
	connect(suddendeath, SIGNAL(timeChanged(const QTime&)), this, SLOT(slotTimeChanged(const QTime&)));

	hbox = new QHBoxLayout();
	label1 = new QLabel(tr("Rating categori:"));
	gametype = new QLabel();
	hbox->addWidget(label1);
	hbox->addWidget(gametype);
	vbox->addLayout(hbox);
	group->setLayout(vbox);
	grid->addWidget(group, 0, 1, 2, 1);

	group = new QGroupBox(frame);
	vbox = new QVBoxLayout();
	hbox = new QHBoxLayout();
	rated = new QCheckBox(tr("Rated game"));
	label1 = new QLabel(tr("Color:"));
	color = new QComboBox();
	color->addItem(tr("White"));
	color->addItem(tr("Black"));
	color->addItem(tr("Random"));
	hbox->addWidget(rated);
	hbox->addWidget(label1);
	hbox->addWidget(color);
	vbox->addLayout(hbox);
	hbox = new QHBoxLayout();
	label1 = new QLabel(tr("Startposition:"));
	startposition = new QLabel(tr("Standard"));
	button1 = new QPushButton("...");
	button1->setMaximumWidth(30);
	hbox->addWidget(label1);
	hbox->addWidget(startposition);
	hbox->addWidget(button1);
	vbox->addLayout(hbox);
	connect(button1, SIGNAL(clicked()), this, SLOT(slotStartposition()));
	group->setLayout(vbox);
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
	QString qs;
	char sz[16];
	setting.color = color->currentIndex();
	setting.player = playername->text();
	setting.startTime = -starttime->time().secsTo(QTime(0, 0, 0));
	setting.startTimeInc = -starttimeinc->time().secsTo(QTime(0, 0, 0));
	if (moves->isChecked())
		setting.suddenDeathTime = -suddendeath->time().secsTo(QTime(0, 0, 0));
	else
		setting.suddenDeathTime = 0;
	setting.rated = rated->isChecked();
	setting.autoskill = autoskill->isChecked();
	setting.fullstrength = fullstrength->isChecked();
	setting.engineskill = skill->text().toInt();
	setting.engineelo = computerelo->text().toInt();
	setting.engineplayer = enginename->text();
	if (skill->isVisible())
		QTextStream(&setting.engineplayer) << ", Skill " << skill->text();
	if (personality->isVisible())
		QTextStream(&setting.engineplayer) << " " << personality->currentText();
	if (autoskill->isVisible() && setting.autoskill)
		QTextStream(&setting.engineplayer) << " Autoskill";

	// Update init parameters for engine
	if (autoskill->isVisible())
		engine->init(engine->autoskill.name, QString(setting.autoskill ? "true" : "false"));
	if (skill->isVisible())
		engine->init(engine->skill.name, QString(skill->text()));
	if (fullstrength->isVisible() && limitstrength->isVisible())
	{
		engine->init(engine->limitstrength.name, QString(setting.fullstrength ? "false" : "true"));
		if (!setting.fullstrength)
			engine->init(engine->elo.name, QString(itoa(setting.engineelo, sz, 10)));
	}

	// Adding book
	if (!engine->ownbook.name.isEmpty())
		engine->init(engine->ownbook.name, QString("true"));
	if (!engine->bookfile.name.isEmpty())
	{
		QString path = QStandardPaths::locate(QStandardPaths::DocumentsLocation, "/Polarchess", QStandardPaths::LocateDirectory);
		QFileInfoList qfil = QDir(path, "pc*.bin").entryInfoList(QDir::NoFilter, QDir::Name | QDir::IgnoreCase);
		int i, j;
		QString bookpath;
		int r = setting.engineelo;
		if (r == 0)
			r = 3000;
		for (i = 1; i <= 3000; i++)
		{
			if ((i > r) && !bookpath.isEmpty())
				break;
			for (j = 0; j < qfil.size(); j++)
			{
				qs = "pc";
				qs+=itoa(i, sz, 10);
				qs += ".bin";
				if (qfil[j].fileName().indexOf(qs) >= 0)
					bookpath = qfil[j].absoluteFilePath();
			}
		}
		if (!bookpath.isEmpty())
		{
			bookpath.replace("/", "\\"); // Is it needed? Kommodo don't care.
			engine->init(engine->bookfile.name, bookpath);
		}
	}
	
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
	char sz[16];
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
	setRating();
}

const NewGameSetting NewGameDialog::getSetting()
{
	return setting;

}

void NewGameDialog::setDefault(NewGameSetting& newsetting)
{
	char sz[16];
	int i;
	//int h, m, s;
	setting = newsetting;
	//engines = eng;
	playername->setText(setting.player);
	enginename->setText(engine->name);

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

	if (engine->personality.name == "Personality")
	{
		for (i = 0; i < engine->personality.combo.var.size(); i++)
			personality->addItem(engine->personality.combo.var[i]);
		personality->setCurrentText(setting.personality);
	}
	else
	{
		personality->setVisible(false);
	}
	if (engine->skill.name == "Skill")
	{
		skill->setMinimum(engine->skill.spin.min);
		skill->setMaximum(engine->skill.spin.max);
		skill->setValue(setting.engineskill);
	}
	else
	{
		skill->setVisible(false);
	}

	if (engine->autoskill.name == "Auto Skill")
		autoskill->setChecked(setting.autoskill);
	else
		autoskill->setVisible(false);

	if (engine->limitstrength.name == "UCI_LimitStrength")
		fullstrength->setChecked(setting.fullstrength);
	else
		fullstrength->setVisible(false);

	if (engine->elo.name == "UCI_Elo")
	{
		limitstrength->setMinimum(engine->elo.spin.min);
		limitstrength->setMaximum(engine->elo.spin.max);
		limitstrength->setValue(setting.engineelo);
	}
	else
	{
		limitstrength->setVisible(false);
	}

	setGameType();

	resize(minimumSizeHint());
}

void NewGameDialog::setRating()
{
	char sz[16];
	int r = 0;
	if (!engine->skill.name.isEmpty())
	{
		if (!engine->autoskill.name.isEmpty() && !engine->autoskill.check.value)
			r = engine->getRating(skill->text().toInt(), personality->currentText(), setting.gameType);
	}
	else if (!engine->limitstrength.name.isEmpty() && engine->limitstrength.check.value)
	{
		r = engine->elo.spin.value;
	}
	setting.engineelo = r;
	computerelo->setText(itoa(r, sz, 10));
}