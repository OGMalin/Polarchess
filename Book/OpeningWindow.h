#pragma once

#include "Database.h"
#include <QLineEdit>

class QLineEdit;
class OpeningWindow : public QLineEdit
{
	Q_OBJECT

public:
	OpeningWindow(QWidget *parent=0);
	~OpeningWindow();
	void update(BookDBEntry& theory, BookDBEntry& rep);

private:
};
