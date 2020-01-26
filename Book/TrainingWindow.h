#pragma once

#include <QWidget>
#include <QString>
#include "Training.h"
#include "StatusWatch.h"
#include <QComboBox>
#include <QCheckBox>

class TrainingWindow : public QWidget
{
	Q_OBJECT

public slots:
	void showContextMenu(const QPoint& pos);
	void selectFont();
	void next();

public:
	Training* trainingDB;
	TrainingWindow(QWidget* parent = 0);
	~TrainingWindow();
	QString fontToString();
	void fontFromString(const QString&);

private:
	StatusWatch* watch;
	QComboBox* colorBox;
	QCheckBox* positionBox;
};