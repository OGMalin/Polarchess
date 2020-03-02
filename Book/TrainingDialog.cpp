#include "TrainingDialog.h"
#include <QBoxLayout>
#include <QPushButton>

TrainingDialog::TrainingDialog(QWidget *parent, QString& tcomment, QString& wcomment, QString& bcomment)
	:QDialog(parent)
{
	setWindowTitle(tr("Training"));
	setMinimumSize(200, 100);
	QPushButton* button;
	QHBoxLayout* hbox = new QHBoxLayout;
	button = new QPushButton(tr("Next"));
	connect(button, SIGNAL(clicked()), this, SLOT(nextClicked()));
	hbox->addWidget(button);
	button = new QPushButton(tr("Stop"));
	connect(button, SIGNAL(clicked()), this, SLOT(stopClicked()));
	hbox->addWidget(button);
	button = new QPushButton(tr("Analyze"));
	connect(button, SIGNAL(clicked()), this, SLOT(analyzeClicked()));
	hbox->addWidget(button);

	setLayout(hbox);
}

void TrainingDialog::nextClicked()
{
	done(1);
}

void TrainingDialog::stopClicked()
{
	done(2);
}

void TrainingDialog::analyzeClicked()
{
	done(3);
}

