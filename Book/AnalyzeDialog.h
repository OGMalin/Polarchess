#pragma once

#include <QDialog>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include "Computer.h"
#include "Database.h"
#include "EngineWindow.h"
#include "Path.h"

class AnalyzeDialog: public QDialog
{
	Q_OBJECT

public slots:
	void startAnalyze();
	void closeAnalyze();
	void enginePV(ComputerDBEngine&, ChessBoard&);

public:
	AnalyzeDialog(QWidget* parent, Computer*, Database*, Database*, Database*, EngineWindow*, Path*);

private:
	QComboBox* dbList;
	QComboBox* engineList;
	QSpinBox* timeToUse;
	QCheckBox* endPosition;
	QCheckBox* currentPath;
	Computer* compDB;
	Database* theoryDB;
	Database* whiteDB;
	Database* blackDB;
	EngineWindow* enginewindow;
	Path* path;
};