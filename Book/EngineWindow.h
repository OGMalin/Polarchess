#pragma once

#include <QWidget>
#include "../Common/Engine.h"

class QPushButton;
class QLineEdit;
class EngineWindow : public QWidget
{
	Q_OBJECT

public:
	EngineWindow(QWidget *parent = 0);
	~EngineWindow();
	void update(ChessBoard& board);

private:
	int multipv;
	Engine* engine;
	QPushButton* analyze;	// Start/stop analysis;
	QPushButton* freeze;	// lock analysis to current position
	QPushButton* incline;	// Increase multipv
	QPushButton* decline;	// Decrease multipv
	QLineEdit* lines;		// Show number of multipv
};

