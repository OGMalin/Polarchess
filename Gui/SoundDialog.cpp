#include "SoundDialog.h"
#include <QGridLayout>
#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QTextToSpeech>

SoundDialog::SoundDialog(QWidget *parent)
	: QDialog(parent)
{

	QGridLayout* grid = new QGridLayout(this); //Main grid
	QHBoxLayout* hbox;
	QVBoxLayout* vbox;
	QLabel* label;
	QTextToSpeech* speech = new QTextToSpeech(this);
	int i;

	hbox = new QHBoxLayout();
	label = new QLabel(tr("Move sound:"));
	usage = new QComboBox();
	usage->addItem(tr("None"));
	usage->addItem(tr("Sound"));
	usage->addItem(tr("Spoken"));
	hbox->addWidget(label);
	hbox->addWidget(usage);
	grid->addLayout(hbox, 0, 0);

	hbox = new QHBoxLayout();
	label = new QLabel(tr("Language:"));
	language = new QComboBox();
	QVector<QLocale> loc = speech->availableLocales();
	for (i = 0; i < loc.size(); i++)
		language->addItem(QLocale::languageToString(loc[i].language()));
	hbox->addWidget(label);
	hbox->addWidget(language);
	grid->addLayout(hbox, 1, 0);

	QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok, this);
	buttons->setOrientation(Qt::Horizontal);
	grid->addWidget(buttons, 2, 0, 1, 1);

	connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
}

SoundDialog::~SoundDialog()
{
}

void SoundDialog::set(SoundSetting setting)
{
	if (setting.language)
	{
		QTextToSpeech* speech = new QTextToSpeech(this);
		int i;
		QVector<QLocale> loc = speech->availableLocales();
		for (i = 0; i < loc.size(); i++)
		{
			if (loc[i].language() == setting.language)
				language->setCurrentIndex(i);
		}
	}
	usage->setCurrentIndex(setting.usage);

}

void SoundDialog::get(SoundSetting& setting)
{
	int index = language->currentIndex();
	QTextToSpeech* speech = new QTextToSpeech(this);
	QVector<QLocale> loc = speech->availableLocales();
	if (index < loc.size())
		setting.language = loc[index].language();
	setting.usage = usage->currentIndex();
}


