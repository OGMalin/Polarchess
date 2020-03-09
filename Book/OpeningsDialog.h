#pragma once

#include <QDialog>
#include <QLineEdit>

class OpeningsDialog :public QDialog
{
public:
	OpeningsDialog(QWidget* parent, QString& eco, QString& name, QString& variation, QString& subvariation, bool newPosition);
	void getItems(QString& eco, QString& name, QString& variation, QString& subvariation);
private:
	QLineEdit* ecoEdit;
	QLineEdit* nameEdit;
	QLineEdit* variationEdit;
	QLineEdit* subvariationEdit;
};
