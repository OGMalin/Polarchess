#pragma once

#include <QWidget>
#include <QString>
#include "Training.h"

class TrainingWindow : public QWidget
{
	Q_OBJECT

public slots:
	void showContextMenu(const QPoint& pos);
	void selectFont();

public:
	Training* training;
	TrainingWindow(QWidget* parent = 0);
	~TrainingWindow();
	QString fontToString();
	void fontFromString(const QString&);

};