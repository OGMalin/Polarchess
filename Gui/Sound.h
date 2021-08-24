#pragma once

#include "../Common/ChessBoard.h"
#include "../Common/ChessMove.h"
#include <QDialog>
#include <QComboBox>
#include <QObject>

struct SoundSetting
{
	int language;
	int usage;
	SoundSetting() { language = usage = 0; };
};

class SoundDialog : public QDialog
{
	Q_OBJECT
private:
	QComboBox* language;
	QComboBox* usage;
public slots:
public:
	SoundDialog(QWidget *parent=NULL);
	~SoundDialog();
	void set(SoundSetting);
	void get(SoundSetting&);
};

class Sound : public QObject
{
	Q_OBJECT
private:
	QString squareAsText(int);
public:
	Sound(QObject* parent);
	~Sound();
	SoundSetting setting;
	void showDialog();
	void say(ChessBoard&, ChessMove&);
};