#pragma once

#include <QDialog>
#include <QStringList>
#include <QString>

class QComboBox;

class PlayerDialog : public QDialog
{
	Q_OBJECT
private:
	QComboBox * playerlist;
	const QStringList getPlayers();
public slots:
	void slotOk(bool);
	void slotAdd();
	void slotRemove();
public:
	PlayerDialog(QWidget *parent, QString& current);
	~PlayerDialog();
};
