#pragma once

#include <QDialog>
#include <QString>

class TrainingDialog : public QDialog
{
	Q_OBJECT

public slots:
	void nextClicked();
	void stopClicked();
	void analyzeClicked();

public:
	TrainingDialog(QWidget *parent, QString& tcomment, QString& wcomment, QString& bcomment);
private:
};