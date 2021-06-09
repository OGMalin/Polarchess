#pragma once

#include <QDialog>
#include "Engine.h"
#include "EnginePlayer.h"

class QLineEdit;
class QComboBox;
class QCheckBox;
class QTimeEdit;
class QLabel;
class QSpinBox;
class QSlider;

enum { BULLET, BLITZ, RAPID, CLASSICAL };
struct NewGameSetting
{
	QString enginename;
	int engineskill=0;
	bool aoutoskill=false;
	QString personality;
	int engineelo=0;
	bool fullstrength=false;
	QString engineplayer; // enginename, engineskill personality
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
	QLineEdit* playername;
	QSpinBox* skill;
	QComboBox* personality;
	QCheckBox* autoskill;
	QCheckBox* fullstrength;
	QSlider* limitstrength;
	QComboBox* computer;
	QComboBox* color;
	QCheckBox* rated;
	QCheckBox* moves;
	QTimeEdit* starttime;
	QTimeEdit* starttimeinc;
	QTimeEdit* suddendeath;
	QLabel* gametype;
	QLabel* computerelo;
	QLabel* startposition;
	QLabel* enginename;

	void setGameType();
private slots:
	void slotOk();
	void slotSelectPlayer();
	void slotTimeChanged(const QTime&);
	void slotPreTime(int);
	void slotStrengthChanged(int);
	void slotStrengthClicked(bool);
	void slotStartposition();

public:
	NewGameDialog(QWidget *parent);
	~NewGameDialog();
	const NewGameSetting getSetting();
	void setDefault(NewGameSetting& newsetting, Engine* eng);
};
