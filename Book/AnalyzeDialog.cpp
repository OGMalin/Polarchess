#include "AnalyzeDialog.h"
#include <QBoxLayout>
#include <QPushButton>

AnalyzeDialog::AnalyzeDialog(QWidget* parent, Computer* c, Database* t, Database* w, Database* b)
	:QDialog(parent)
{
	QHBoxLayout* hbox;
	QVBoxLayout* vbox;
	QPushButton* button;
	compDB = c;
	theoryDB = t;
	whiteDB = w;
	blackDB = b;
	
	vbox = new QVBoxLayout;

	hbox = new QHBoxLayout;
	engineList = new QComboBox;
	dbList = new QComboBox;
	hbox->addWidget(engineList);
	hbox->addWidget(dbList);
	vbox->addLayout(hbox);

	setLayout(vbox);
}