#pragma once

#include <QWidget>
#include <QString>
class TrainingWindow : public QWidget
{
	Q_OBJECT

public:
	TrainingWindow(QWidget* parent = 0);
	~TrainingWindow();
	QString fontToString();
	void fontFromString(const QString&);
};