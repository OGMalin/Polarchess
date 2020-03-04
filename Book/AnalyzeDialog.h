#pragma once

#include <QDialog>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QVector>
#include <QLabel>
#include "../Common/BoardWindow.h"
#include "Computer.h"
#include "Database.h"
#include "Training.h"
#include "EngineWindow.h"
#include "Path.h"

class AnalyzeDialog: public QDialog
{
	Q_OBJECT

public slots:
	void startAnalyze();
	void stopAnalyze();
	void closeAnalyze();
	void pathChanged(int);
	void enginePV(ComputerDBEngine&, ChessBoard&);

signals:
	void setPosition(const ChessBoard&);

public:
	AnalyzeDialog(QWidget* parent, Computer*, Database*, Database*, Database*, EngineWindow*, BoardWindow*, Path*, Training*);

private:
	void collectPositions();
	void updateAnalyzed();
	QComboBox* dbList;
	QComboBox* engineList;
	QSpinBox* timeToUse;
	QCheckBox* endPosition;
	QCheckBox* currentPath;
	QPushButton* startButton;
	QPushButton* stopButton;
	QPushButton* closeButton;
	QLabel* posLabel;
	Computer* compDB;
	Database* theoryDB;
	Database* whiteDB;
	Database* blackDB;
	Training* trainingDB;
	EngineWindow* enginewindow;
	BoardWindow* boardwindow;
	QVector<ChessBoard> positions;
	Path* path;
	bool running;
	int currentPosition;
};