#pragma once

#include "Training.h"
#include "Watch.h"
#include "Database.h"
#include "Computer.h"
#include "../Common/ChessBoard.h"
#include "../Common/MoveList.h"
#include <QWidget>
#include <QString>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QVector>
#include <QTextEdit>
#include <QColor>

class TrainingWindow : public QWidget
{
	Q_OBJECT

public slots:
	void showContextMenu(const QPoint& pos);
	void selectFont();
	void start();
	void next();

signals:
//	void trainingNext(ChessBoard&, int color);
	void trainingFlipBoard(int color);
	void trainingAddMoves(MoveList&);
	void trainingSetArrow(int fromSq, int toSq, bool wrong, int sek);
	void nextResponse();
	void restart();
	void trainingStop();
public:
	Training* training;
	TrainingWindow(QWidget* parent = 0);
	~TrainingWindow();
	QString fontToString();
	void fontFromString(const QString&);
	void setCurrentBoard(const ChessBoard&);
	void setTraining(Training* t);
	void updateStat(int cur = 0);
	void moveEntered(ChessMove& move);
	bool isRunning() { return running; };
	void stopRunning();
	void setDatabase(Database* t, Database* w, Database* b, Computer* c);
private:
	Database* Base[3];
	Computer* compDB;
	Watch* watch;
	QComboBox* colorBox;
	QCheckBox* positionBox;
	QPushButton* startButton;
	QLabel*  lRunning;
	QLabel* inBase;
	QLabel* loaded;
//	QLabel* rowid;
	QLabel* score;
	QLabel* computerScore;
	QTextEdit* comment;
	ChessBoard currentBoard;
	TrainingLine trainingLine;
	bool running;
	QColor theoryColor;
	QColor repColor;
	void updateComment(bool visible);
};