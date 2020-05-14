#pragma once

#include <QDialog>
#include <QComboBox>
#include <QSpinBox>
#include <QRadioButton>
#include <QPushButton>
#include <QVector>
#include <QLabel>
#include "../Common/BoardWindow.h"
#include "Computer.h"
#include "Database.h"
#include "EngineWindow.h"
#include "Path.h"

class AnalyzeDialog: public QDialog
{
	Q_OBJECT

public slots:
	void startAnalyze();
	void stopAnalyze();
	void closeAnalyze();
	void pathChanged(bool);
	void enginePV(ComputerDBEngine&, ChessBoard&);

signals:
	void setPosition(const ChessBoard&);

public:
	AnalyzeDialog(QWidget* parent, Computer*, Database*, Database*, Database*, EngineWindow*, BoardWindow*, Path*);

private:
	void collectPositions();
	void updateAnalyzed();
	QComboBox* dbList;
	QComboBox* engineList;
	QSpinBox* timeToUse;
	QRadioButton* allPosition;
	QRadioButton* endPosition;
	QRadioButton* currentPath;
	QPushButton* startButton;
	QPushButton* stopButton;
	QPushButton* closeButton;
	QLabel* posLabel;
	Computer* compDB;
	Database* base[3];
	EngineWindow* enginewindow;
	BoardWindow* boardwindow;
	QVector<ChessBoard> positions;
	Path* path;
	bool running;
	int currentPosition;
};