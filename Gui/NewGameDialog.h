#pragma once

#include <QDialog>

class QLineEdit;
class QComboBox;
class QCheckBox;
class QTimeEdit;
class QLabel;

enum { BULLET, BLITZ, RAPID, CLASSICAL };
struct NewGameSetting
{
	QString computer;
	QString player;
	int gameType;
	int startTime;
	int startTimeInc;
	int suddenDeathTime;
	bool rated;
	// 0=white, 1=black, 2=random
	int color;
};

class NewGameDialog : public QDialog
{
	Q_OBJECT

private:
	NewGameSetting setting;
	QLineEdit* playername;
	QComboBox* computer;
	QComboBox* color;
	QCheckBox* rated;
	QCheckBox* moves;
	QTimeEdit* starttime;
	QTimeEdit* starttimeinc;
	QTimeEdit* suddendeath;
	QLabel* gametype;
	QLabel* computerelo;
	
	const QStringList getEnginePlayers();
	void setGameType();
private slots:
	void slotOk(bool);
	void slotSelectPlayer();
	void slotTimeChanged(const QTime&);
	void slotPreTime(int);
	void slotComputerChanged(int);

public:
	NewGameDialog(QWidget *parent);
	~NewGameDialog();
	const NewGameSetting getSetting();
	void setDefault(const NewGameSetting& newsetting);
};
