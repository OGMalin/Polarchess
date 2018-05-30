#include "AboutDialog.h"
#include <QPushButton>
#include <QGridLayout>

AboutDialog::AboutDialog(QWidget *parent)
	: QDialog(parent)
{
	QGridLayout* grid = new QGridLayout;
	resize(300, 200);
	QPushButton* ok = new QPushButton("Ok", this);
	ok->setDefault(true);

	grid->addWidget(ok, 1, 1,Qt::AlignRight|Qt::AlignBottom);

	setLayout(grid);

	connect(ok, SIGNAL(clicked()), this, SLOT(accept()));
	setSizeGripEnabled(true);
}

AboutDialog::~AboutDialog()
{
}

