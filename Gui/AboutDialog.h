#pragma once
//#include <QWidget>
#include <QDialog>

class AboutDialog : public QDialog
{
	Q_OBJECT

private slots:
	void slotOk(bool);

public:
	AboutDialog(QWidget *parent);
	~AboutDialog();
};
