#pragma once

#include "Database.h"
#include <QWidget>

class QLineEdit;
class OpeningWindow : public QWidget
{
	Q_OBJECT

public:
	OpeningWindow(QWidget *parent=0);
	~OpeningWindow();
	void update(BookDBEntry& theory, BookDBEntry& rep);

private:
};
