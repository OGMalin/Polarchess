#include "OpeningsDialog.h"
#include <QBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>

OpeningsDialog::OpeningsDialog(QWidget* parent, QString& eco, QString& name, QString& variation, QString& subvariation, bool newPosition)
	:QDialog(parent)
{
	QVBoxLayout* vbox = new QVBoxLayout;
	QLabel* label = new QLabel(tr("Eco"));
	vbox->addWidget(label);
	ecoEdit = new QLineEdit(eco);
	vbox->addWidget(ecoEdit);
	label = new QLabel(tr("Opening"));
	vbox->addWidget(label);
	nameEdit = new QLineEdit(name);
	vbox->addWidget(nameEdit);
	label = new QLabel(tr("Variation"));
	vbox->addWidget(label);
	variationEdit = new QLineEdit(variation);
	vbox->addWidget(variationEdit);
	label = new QLabel(tr("Subvariation"));
	vbox->addWidget(label);
	subvariationEdit = new QLineEdit(subvariation);
	vbox->addWidget(subvariationEdit);
	label = new QLabel();
	if (newPosition)
		label->setText(tr("New position"));
	else
		label->setText(tr("Existing position"));
	vbox->addWidget(label);
	QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel, this);
	vbox->addWidget(buttons);

	setLayout(vbox);

	connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
}

void OpeningsDialog::getItems(QString& eco, QString& name, QString& variation, QString& subvariation)
{
	eco = ecoEdit->text();
	name = nameEdit->text();
	variation = variationEdit->text();
	subvariation = subvariationEdit->text();
}