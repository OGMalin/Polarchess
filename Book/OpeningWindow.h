#pragma once

#include <QWidget>

class QLineEdit;
class OpeningWindow : public QWidget
{
	Q_OBJECT

public:
	OpeningWindow(QWidget *parent=0);
	~OpeningWindow();

private:
	QLineEdit* opening;
	QLineEdit* variation;
	QLineEdit* subvariation;
	QLineEdit* eco;

};
