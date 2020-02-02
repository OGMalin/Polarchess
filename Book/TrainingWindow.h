#pragma once

#include "Training.h"
#include "StatusWatch.h"
#include "../Common/ChessBoard.h"
#include "../Common/MoveList.h"
#include <QWidget>
#include <QString>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QVector>

class TrainingWindow : public QWidget
{
	Q_OBJECT

public slots:
	void showContextMenu(const QPoint& pos);
	void selectFont();
	void next();

signals:
//	void trainingNext(ChessBoard&, int color);
	void trainingFlipBoard(int color);
	void trainingAddMoves(MoveList&);
	void trainingSetArrow(int fromSq, int toSq, bool wrong, int sek);
public:
	Training* trainingDB;
	TrainingWindow(QWidget* parent = 0);
	~TrainingWindow();
	QString fontToString();
	void fontFromString(const QString&);
	void setCurrentBoard(const ChessBoard&);
	void setTrainingDB(Training* db);
	void updateStat();
	void moveEntered(ChessMove& move);
	bool isRunning() { return running; };
private:
	StatusWatch* watch;
	QComboBox* colorBox;
	QCheckBox* positionBox;
	QLabel*  lRunning;
	QLabel* inBase;
	QLabel* loaded;
	ChessBoard currentBoard;
	TrainingDBEntry trainingLine;
	bool running;
};