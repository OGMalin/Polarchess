#include "NewGameDialog.h"
#include "PlayerDialog.h"
#include <QGridLayout>
#include <QPushButton>
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

NewGameDialog::NewGameDialog(QWidget *parent)
	: QDialog(parent)
{
	setWindowTitle(tr("New game"));
	setSizeGripEnabled(true);
	QGridLayout* grid = new QGridLayout;
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

	group = new QGroupBox(tr("Your name"));
	playername = new QLineEdit(group);
	playername->setReadOnly(true);
	hbox = new QHBoxLayout;
	hbox->addWidget(playername);
	button1 = new QPushButton(tr("Change player"));
	hbox->addWidget(button1);
	hbox->addStretch(1);
	group->setLayout(hbox);
	grid->addWidget(group, 0, 0, Qt::AlignLeft);
	connect(button1, SIGNAL(clicked()), this, SLOT(slotSelectPlayer()));

	group = new QGroupBox(tr("Opponent"));
	computer = new QComboBox;
	computer->addItems(getEnginePlayers());
	hbox = new QHBoxLayout;
	hbox->addWidget(computer);
	computerelo = new QLabel();
	hbox->addWidget(computerelo);
	hbox->addStretch(1);
	group->setLayout(hbox);
	grid->addWidget(group, 1, 0, Qt::AlignLeft);
	connect(computer, SIGNAL(currentIndexChanged(int)), this, SLOT(slotComputerChanged(int)));

	rated = new QCheckBox(tr("Rated game"));
	grid->addWidget(rated, 2, 0, Qt::AlignLeft);

	group = new QGroupBox(tr("Color"));
	color = new QComboBox;
	color->addItem(tr("White"));
	color->addItem(tr("Black"));
	color->addItem(tr("Random"));
	hbox = new QHBoxLayout;
	hbox->addWidget(color);
	hbox->addStretch(1);
	group->setLayout(hbox);
	grid->addWidget(group, 3, 0, Qt::AlignLeft);

	group = new QGroupBox(tr("Timecontrol"));
	QGridLayout* grid2 = new QGridLayout;
	label5 = new QLabel(tr("Predefined timecontrols:"));
	combo = new QComboBox;
	combo->addItem(tr(""));
	combo->addItem(tr("90 min. + 30 sec."));
	combo->addItem(tr("30 min."));
	combo->addItem(tr("15 min. + 10 sec."));
	combo->addItem(tr("5 min."));
	combo->addItem(tr("3 min. + 2 sec."));
	combo->addItem(tr("1 min. + 2 sec."));
	label1 = new QLabel(tr("Start time:"));
	starttime = new QTimeEdit;
	starttime->setDisplayFormat("HH::mm:ss");
	starttime->setMinimumTime(QTime(0, 0, 30));
	label2 = new QLabel(tr("Increment:"));
	starttimeinc = new QTimeEdit;
	starttimeinc->setDisplayFormat("mm:ss");
	starttimeinc->setMaximumTime(QTime(0, 59, 59));
	starttimeinc->setCurrentSection(QDateTimeEdit::MinuteSection);
	moves = new QCheckBox(tr("New time at 40 moves"));
	label3 = new QLabel(tr("Sudden death:"));
	suddendeath = new QTimeEdit;
	suddendeath->setDisplayFormat("HH::mm:ss");
	label4 = new QLabel(tr("Rating categori:"));
	gametype = new QLabel();
	grid2->addWidget(label5, 0, 0, Qt::AlignLeft);
	grid2->addWidget(combo, 0, 1, Qt::AlignLeft);
	grid2->addWidget(label1, 1, 0, Qt::AlignLeft);
	grid2->addWidget(starttime, 1, 1, Qt::AlignRight);
	grid2->addWidget(label2, 2, 0, Qt::AlignLeft);
	grid2->addWidget(starttimeinc, 2, 1, Qt::AlignRight);
	grid2->addWidget(moves, 3, 0, Qt::AlignLeft);
	grid2->addWidget(label3, 4, 0, Qt::AlignLeft);
	grid2->addWidget(suddendeath, 4, 1, Qt::AlignRight);
	grid2->addWidget(label4, 5, 0, Qt::AlignLeft);
	grid2->addWidget(gametype, 5, 1, Qt::AlignLeft);
	group->setLayout(grid2);
	grid->addWidget(group, 0, 1, 3, 1, Qt::AlignLeft);
	connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(slotPreTime(int)));
	connect(starttime, SIGNAL(timeChanged(const QTime&)), this, SLOT(slotTimeChanged(const QTime&)));
	connect(starttimeinc, SIGNAL(timeChanged(const QTime&)), this, SLOT(slotTimeChanged(const QTime&)));
	connect(moves, SIGNAL(clicked(bool)), suddendeath, SLOT(setEnabled(bool)));
	connect(suddendeath, SIGNAL(timeChanged(const QTime&)), this, SLOT(slotTimeChanged(const QTime&)));

	hbox = new QHBoxLayout;
	button1 = new QPushButton(tr("Cancel"));
	button2 = new QPushButton(tr("Ok"));
	hbox->addWidget(button2);
	hbox->addWidget(button1);
	grid->addLayout(hbox, 4, 1, Qt::AlignRight | Qt::AlignBottom);
	connect(button1, SIGNAL(clicked()), this, SLOT(reject()));
	connect(button2, SIGNAL(clicked(bool)), this, SLOT(slotOk(bool)));

	setLayout(grid);

}

NewGameDialog::~NewGameDialog()
{
}

void NewGameDialog::slotOk(bool)
{
	setting.color = color->currentIndex();
	setting.computer = computer->currentText();
	setting.player = playername->text();
	setting.startTime = -starttime->time().secsTo(QTime(0, 0, 0));
	setting.startTimeInc = -starttimeinc->time().secsTo(QTime(0, 0, 0));
	if (moves->isChecked())
		setting.suddenDeathTime = -suddendeath->time().secsTo(QTime(0, 0, 0));
	else
		setting.suddenDeathTime = 0;
	setting.rated = rated->isChecked();
	accept();
}

void NewGameDialog::slotSelectPlayer()
{
	PlayerDialog dialog(this, playername->text());
	if (dialog.exec() == QDialog::Rejected)
		return;
}

void NewGameDialog::slotTimeChanged(const QTime&)
{
	setGameType();
}

void NewGameDialog::slotComputerChanged(int)
{
	static bool needread = true;
	int i;
	static QMap<QString, QString> map;
	QString comp = setting.computer = computer->currentText();
	if (comp.isEmpty())
		return;
	if (needread)
	{
#ifdef _DEBUG
		QFile file("../x64/Debug/personalities/elo.ini");
#else
		QFile file("personalities/elo.ini");
#endif
		if (!file.open(QFile::ReadOnly | QFile::Text))
			return;
		QTextStream in(&file);
		QString line;
		while (!in.atEnd())
		{
			line = in.readLine();
			i = line.lastIndexOf("=");
			if (i > 1)
				map.insert(line.left(i).trimmed(), line.mid(i + 1).trimmed());
		}
		file.close();
	}

	QString val = map[comp];
	computerelo->setText("Elo:\n" + val);
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

void NewGameDialog::setDefault(const NewGameSetting& newsetting)
{
	int h, m, s;
	setting = newsetting;
	if (setting.player.isEmpty())
	{
		setting.player = getenv("USER");
		if (setting.player.isEmpty())
			setting.player = getenv("USERNAME");
		setting.startTime = 45 * 60;
		setting.startTimeInc = 45;
		setting.suddenDeathTime = 0;
		setting.rated = false;
	}
	playername->setText(setting.player);
	if (!setting.computer.isEmpty())
		computer->setCurrentText(setting.computer);

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

	setGameType();
	slotComputerChanged(0);
	resize(minimumSizeHint());
}

const QStringList NewGameDialog::getEnginePlayers()
{
#ifdef _DEBUG
	QDir dir("../x64/Debug/personalities");
#else
	QDir dir("personalities");
#endif

	if (!dir.exists())
		return QStringList();
	QStringList list;
	list.append("*.per");
	dir.setSorting(QDir::IgnoreCase);
	dir.setNameFilters(list);
	list = dir.entryList();
	list.replaceInStrings(".per", "", Qt::CaseInsensitive);
	return list;
}

