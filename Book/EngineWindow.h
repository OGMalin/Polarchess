#pragma once

#include <QWidget>
#include "../Common/Engine.h"
#include "../Common/ChessBoard.h"
#include "../Common/MoveList.h"
#include <QStandardItemModel>
#include <QFont>
#include <QComboBox>

class QPushButton;
class QLabel;
class EngineWindow : public QWidget
{
	Q_OBJECT

public:
	QString iniPath;
	EngineWindow(QWidget *parent = 0);
	~EngineWindow();
	void setPosition(const ChessBoard& cb, int movenr=1);

public slots:
	void analyzeClicked(bool);
	void freezeClicked(bool);
	void inclineClicked(bool);
	void declineClicked(bool);
	void engineReady();
	void engineStoped();
	void engineInfo(const EngineInfo&);
	void showContextMenu(const QPoint& pos);
	void selectFont();
	void selectEngine(const QString& eng);

private:
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

