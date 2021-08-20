#pragma once

#include <QDialog>
#include <QComboBox>

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
	SoundDialog(QWidget *parent);
	~SoundDialog();
	void set(SoundSetting);
	void get(SoundSetting&);
};
