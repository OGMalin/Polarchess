#pragma once

#include <QWidget>
#include "../Common/Engine.h"
#include "../Common/ChessBoard.h"
#include "../Common/MoveList.h"
#include <QStandardItemModel>

class QPushButton;
class QLabel;
class EngineWindow : public QWidget
{
	Q_OBJECT

public:
	QString iniPath;
	EngineWindow(QWidget *parent = 0);
	~EngineWindow();
	void setPosition(const ChessBoard& cb);

public slots:
	void analyzeClicked(bool);
	void freezeClicked(bool);
	void inclineClicked(bool);
	void declineClicked(bool);
	void engineReady();
	void engineStoped(const QString& move, const QString& ponder);
	void engineInfo(const EngineInfo&);

private:
	int multipv;
	QString engineName;
	Engine* engine;
	QPushButton* analyze;	// Start/stop analysis;
	QPushButton* freeze;	// lock analysis to current position
	QPushButton* incline;	// Increase multipv
	QPushButton* decline;	// Decrease multipv
	QLabel* lines;		// Show number of multipv
	QStandardItemModel* model;
	bool analyzing;
	bool freezing;
	ChessBoard currentBoard;
	ChessBoard freezeBoard;
	MoveList movelist;
};

