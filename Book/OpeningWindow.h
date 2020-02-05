#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QString>
#include "Path.h"
#include "Openings.h"

class OpeningWindow : public QWidget
{
	Q_OBJECT
public:
	OpeningWindow(QWidget *parent=0);
	~OpeningWindow();
	void refresh(Path*);
	void setOpeningsDB(Openings* db) { openingsDB = db; };
	QString fontToString();
	void fontFromString(const QString&);

public slots:
	void showContextMenu(const QPoint& pos);
	void selectFont();
	void editLine();

private:
	QLineEdit* openingline;
	Openings* openingsDB;
	OpeningsDBEntry ode;
};
