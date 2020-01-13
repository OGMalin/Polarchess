#include "ListOrderDialog.h"
#include <QDialogButtonBox>
#include <QVBoxLayout>

ListOrderDialog::ListOrderDialog(QString& label, QWidget* parent)
	:QDialog(parent)
{

	setWindowTitle(label);

	QVBoxLayout* vbox = new QVBoxLayout;
	list = new QListWidget(this);
	list->setDragDropMode(QAbstractItemView::InternalMove);
	QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
	vbox->addWidget(list);
	vbox->addWidget(buttons);
	setLayout(vbox);

	connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
}

void ListOrderDialog::setList(const QStringList& l)
{
	list->addItems(l);
}

QStringList ListOrderDialog::getList()
{
	QStringList l;
	int row = 0;
	QListWidgetItem* item;
	while ((item = list->item(row)) != NULL)
	{
		l.push_back(item->text());
		++row;
	}
	return l;
}
