#include "TrainingWindow.h"
#include <QMenu>
#include <QFontDialog>
#include <QBoxLayout>
#include <QPushButton>
#include <QTextStream>

TrainingWindow::TrainingWindow(QWidget* parent)
	: QWidget(parent)
{
	QPushButton* button;
	QVBoxLayout* vbox;
	QHBoxLayout* hbox;

	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));

	vbox = new QVBoxLayout;

	hbox = new QHBoxLayout;
	watch = new StatusWatch;
	inBase = new QLabel(tr("In base: "));
	loaded = new QLabel(tr("Loaded: "));
	hbox->addWidget(watch);
	hbox->addWidget(inBase);
	hbox->addWidget(loaded);
	vbox->addLayout(hbox);

	hbox = new QHBoxLayout;
	colorBox = new QComboBox;
	colorBox->addItem(tr("Both"));
	colorBox->addItem(tr("White"));
	colorBox->addItem(tr("Black"));
	colorBox->setFixedWidth(colorBox->minimumSizeHint().width());
	hbox->addWidget(colorBox);
	positionBox = new QCheckBox(tr("From current position"));
	hbox->addWidget(positionBox);
	vbox->addLayout(hbox);

	hbox = new QHBoxLayout;
	button = new QPushButton(tr("Stop training"));
	connect(button, SIGNAL(clicked()), this, SLOT(next()));
	button->setFixedWidth(button->minimumSizeHint().width());
	hbox->addWidget(button);
	button = new QPushButton(tr("Next"));
	connect(button, SIGNAL(clicked()), this, SLOT(next()));
	button->setFixedWidth(button->minimumSizeHint().width());
	hbox->addWidget(button);

	vbox->addLayout(hbox);
	vbox->setAlignment(Qt::AlignRight);
	setLayout(vbox);
}

TrainingWindow::~TrainingWindow()
{
}

void TrainingWindow::showContextMenu(const QPoint& pos)
{
	QMenu* contextMenu = new QMenu(this);
	contextMenu->addAction(QString("Font"), this, SLOT(selectFont()));
	contextMenu->exec(mapToGlobal(pos));
}

void TrainingWindow::selectFont()
{
	bool ok;
	QFont f = QFontDialog::getFont(&ok, font(), this);
	if (ok)
	{
		setFont(f);
	}
}

QString TrainingWindow::fontToString()
{
	return font().toString();
}

void TrainingWindow::fontFromString(const QString& sFont)
{
	if (sFont.isEmpty())
		return;
	QFont f;
	f.fromString(sFont);
	setFont(f);
}

void TrainingWindow::next()
{
	ChessBoard cb;
	int color = colorBox->currentIndex()-1;
	if (positionBox->isChecked())
		cb = currentBoard;
	else
		cb.setStartposition();
	emit trainingNext(cb, color);
	updateStat();
}

void TrainingWindow::setCurrentBoard(const ChessBoard& cb)
{
	currentBoard = cb;
}

void TrainingWindow::setTrainingDB(Training* db)
{
	trainingDB = db;
}

void TrainingWindow::updateStat()
{
	TrainingStatistics ts = trainingDB->getStat();
	QString qs = tr("In base: ");
	QTextStream(&qs) << ts.inBase;
	inBase->setText(qs);
	qs = tr("Loaded: ");
	QTextStream(&qs) << ts.inBase;
	loaded->setText(qs);
}