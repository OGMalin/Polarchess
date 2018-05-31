#pragma once

#include <QDialog>

class QListWidget;

class PlayerDialog : public QDialog
{
	Q_OBJECT
private:
	QListWidget * playerlist;
public slots:
	void slotOk(bool);
public:
	PlayerDialog(QWidget *parent);
	~PlayerDialog();
};
