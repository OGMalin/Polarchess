#include "AboutDialog.h"
#include <QPushButton>
#include <QGridLayout>

AboutDialog::AboutDialog(QWidget *parent)
	: QDialog(parent)
{
	QGridLayout* grid = new QGridLayout;
	
	QPushButton* ok = new QPushButton("Ok", this);
	ok->setDefault(true);

	grid->addWidget(ok, 1, 1,Qt::AlignRight|Qt::AlignBottom);

	setLayout(grid);

	connect(ok, SIGNAL(clicked(bool)), this, SLOT(slotOk(bool)));
	setSizeGripEnabled(true);
}

AboutDialog::~AboutDialog()
{
}

void AboutDialog::slotOk(bool)
{
	accept();
}