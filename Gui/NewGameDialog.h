#pragma once

#include <QDialog>
#include "EnginePlayer.h"

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
	int startTime=900;
	int startTimeInc=10;
	int suddenDeathTime=0;
	bool rated=false;
	// 0=white, 1=black, 2=random
	int color=2;
};

class NewGameDialog : public QDialog
{
	Q_OBJECT

private:
	NewGameSetting setting;
	EnginePlayers engines;
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
	void setDefault(const NewGameSetting& newsetting, const EnginePlayers& eng);
};
