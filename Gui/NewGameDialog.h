#pragma once

#include <QDialog>

class QLineEdit;

enum { BULLET, BLITZ, RAPID, CLASSICAL };
struct NewGameSetting
{
	QString engineName;
	QString player;
	int gameType;
	int moves;
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
	const QStringList& getEnginePlayers();
private slots:
	void slotOk(bool);
	void slotSelectPlayer();

public:
	NewGameDialog(QWidget *parent);
	~NewGameDialog();
	const NewGameSetting getSetting();
	void setDefault(const NewGameSetting& newsetting);
};
