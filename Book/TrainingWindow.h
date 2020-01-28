#pragma once

#include "Training.h"
#include "StatusWatch.h"
#include "../Common/ChessBoard.h"
#include <QWidget>
#include <QString>
#include <QComboBox>
#include <QCheckBox>

class TrainingWindow : public QWidget
{
	Q_OBJECT

public slots:
	void showContextMenu(const QPoint& pos);
	void selectFont();
	void next();

signals:
	void trainingNext(ChessBoard&, int color);

public:
	Training* trainingDB;
	TrainingWindow(QWidget* parent = 0);
	~TrainingWindow();
	QString fontToString();
	void fontFromString(const QString&);
	void setCurrentBoard(const ChessBoard&);

private:
	StatusWatch* watch;
	QComboBox* colorBox;
	QCheckBox* positionBox;
	ChessBoard currentBoard;
};