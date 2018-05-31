#include "AboutDialog.h"
#include <QPushButton>
#include <QGridLayout>
#include <QCoreApplication>
#include <QLabel>

AboutDialog::AboutDialog(QWidget *parent)
	: QDialog(parent)
{
	QGridLayout* grid = new QGridLayout;

	QPushButton* ok = new QPushButton("Ok", this);
	ok->setDefault(true);
	QString s;
	s = QCoreApplication::organizationName();
	s += "\n";
	s += QCoreApplication::applicationName();
	s += " version: ";
	s += QCoreApplication::applicationVersion();

	QLabel* label = new QLabel(s);
	grid->addWidget(label, 0, 0, 1, 2, Qt::AlignLeft);

	grid->addWidget(ok, 1, 1,Qt::AlignRight|Qt::AlignBottom);

	setLayout(grid);

	connect(ok, SIGNAL(clicked()), this, SLOT(accept()));
	setSizeGripEnabled(true);
	resize(minimumSizeHint());
}

AboutDialog::~AboutDialog()
{
}

