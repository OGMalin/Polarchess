#pragma once

#include <QWidget>
#include "../Common/Engine.h"
#include "../Common/ChessBoard.h"
#include "../Common/MoveList.h"
#include <QStandardItemModel>
#include <QFont>
#include <QComboBox>
#include "Computer.h"

class QPushButton;
class QLabel;
class EngineWindow : public QWidget
{
	Q_OBJECT

public:
	QString iniPath;
	EngineWindow(QWidget *parent = 0);
	~EngineWindow();
	void setPosition(ChessBoard& cb, int movenr=1);

public slots:
	void slotAnalyzeClicked(bool);
	void slotFreezeClicked(bool);
	void slotInclineClicked(bool);
	void slotDeclineClicked(bool);
	void slotEngineInfo(const EngineInfo&);
	void slotShowContextMenu(const QPoint& pos);
	void slotSelectFont();
	void slotSelectEngine(const QString& eng);
	void slotEngineStarted();
	void slotEngineStoped();
signals:
	void enginePV(ComputerDBEngine&);
private:
	unsigned int timeLimit;
	bool engineReady;
	int multipv;
	QString engineName;
	Engine* engine;
	QPushButton* analyze;	// Start/stop analysis;
	QPushButton* freeze;	// lock analysis to current position
	QPushButton* incline;	// Increase multipv
	QPushButton* decline;	// Decrease multipv
	QLabel* lines;		// Show number of multipv
	QLabel* nodes;
	QLabel* nps;
	QLabel* time;
	QComboBox* selengine;
	QStandardItemModel* model;
	bool analyzing;
	bool freezing;
	ChessBoard currentBoard;
	ChessBoard freezeBoard;
	MoveList movelist;
	int movenr;
	int freezemovenr;
	QFont font;
};

