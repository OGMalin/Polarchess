#include "TrainingWindow.h"
#include <QMenu>
#include <QFontDialog>
#include <QBoxLayout>
#include <QPushButton>

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
	hbox->addWidget(watch);
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

}