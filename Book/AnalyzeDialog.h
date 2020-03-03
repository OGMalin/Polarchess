#pragma once

#include <QDialog>
#include <QComboBox>
#include "Computer.h"
#include "Database.h"

class AnalyzeDialog: public QDialog
{
	Q_OBJECT
public:
	AnalyzeDialog(QWidget* parent, Computer*, Database*, Database*, Database*);

private:
	QComboBox* dbList;
	QComboBox* engineList;
	Computer* compDB;
	Database* theoryDB;
	Database* whiteDB;
	Database* blackDB;
};