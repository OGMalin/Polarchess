#include "Sound.h"
#include "../Common/defs.h"
#include <QGridLayout>
#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QSound>

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
	delete speech;
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


Sound::Sound(QObject* parent)
	:QObject(parent)
{
	speech = new QTextToSpeech(this);
}

Sound::~Sound()
{

}

void Sound::showDialog()
{
	SoundDialog dialog;// (this);
	dialog.set(setting);
	if (dialog.exec() == QDialog::Rejected)
		return;
	dialog.get(setting);
}

void Sound::say(ChessBoard& cb, ChessMove& cm)
{
	if (setting.usage == 0) // No sound
		return;

	if (setting.usage == 1) // Play a sound
	{

		return;
	}

	int piece = PIECE(cb.board[cm.fromSquare]);
	QString text;
	if (cm.moveType == CASTLE)
	{
		if (FILE(cm.toSquare) == 2)
			text = "Castle Queenside";
		else
			text = "Castle Kingside";
	}
	else
	{
		text = pieceAsText(piece);
		text += " from ";
		text += squareAsText(cm.fromSquare);
		text += " to ";
		text += squareAsText(cm.toSquare);
	}
	//if (setting.language)
	//	speech->setLocale((QLocale::Language)setting.language);
	//		QLocale loc((QLocale::Language)setting.language);
	speech->say(text);
}

QString Sound::squareAsText(int sq)
{
	char* files = "abcdefgh";
	char* ranks = "12345678";
	char res[3];
	int f = FILE(sq);
	int r = RANK(sq);
	if ((f > 7) || (f < 0))
		return "";
	if ((r > 7) || (r < 0))
		return "";
	res[0] = files[f];
	res[1] = ranks[r];
	res[2] = 0;
	return res;
}

QString Sound::pieceAsText(int p)
{
	char* pieces[] = { "", "Pawn", "Knight", "Bishop", "Rook", "Queen", "King" };
	if ((p > 6) || (p < 1))
		return "";
	return pieces[p];
}
