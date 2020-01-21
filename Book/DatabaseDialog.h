#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QString>
#include "Statistics.h"
#include "Database.h"
#include "Training.h"
#include "Computer.h"

class DatabaseDialog : public QDialog
{
	Q_OBJECT

public slots:
	void openTheoryDB();
	void newTheoryDB();
	void closeTheoryDB();
	void importPGNTheory();
	void importBookTheory();

	void openWhiteDB();
	void newWhiteDB();
	void closeWhiteDB();
	void importPGNWhite();
	void importBookWhite();

	void openBlackDB();
	void newBlackDB();
	void closeBlackDB();
	void importPGNBlack();
	void importBookBlack();

	void openTrainingDB();
	void newTrainingDB();
	void closeTrainingDB();
	void clearTrainingDB();
	void createTrainingDB();

	void openComputerDB();
	void newComputerDB();
	void closeComputerDB();

	void openStatisticDB();
	void newStatisticDB();
	void closeStatisticDB();
	void importStatisticsDB();
	void compactStatisticsDB();
public:
	DatabaseDialog(QWidget *parent, Database* theory, Database* white, Database* black, Training*, Computer*, Statistics*);
//	void getItems(QString& theory, QString& white, QString& black, QString& training, QString& computer, QString& statistics);
//	void setItems(const QString& theorypath, const QString& whitepath, const QString& blackpath, const QString& trainingpath, const QString& computerpath, const QString& statisticspath);
private:
	QLineEdit* theoryFile;
	QLineEdit* whiteFile;
	QLineEdit* blackFile;
	QLineEdit* trainingFile;
	QLineEdit* computerFile;
	QLineEdit* statisticFile;
	Statistics* statDB;
	Database* theoryDB;
	Database* whiteDB;
	Database* blackDB;
	Training* trainingDB;
	Computer* computerDB;
	//bool theoryChanged;
	//bool whiteChanged;
	//bool blackChanged;
	//bool statisticChanged;
};

