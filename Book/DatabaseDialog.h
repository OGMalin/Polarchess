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
	void openWhiteDB();
	void openBlackDB();
	void openTrainingDB();
	void openComputerDB();
	void openStatisticDB();
	void newTheoryDB();
	void newWhiteDB();
	void newBlackDB();
	void newTrainingDB();
	void newComputerDB();
	void newStatisticDB();
	void closeTheoryDB();
	void closeWhiteDB();
	void closeBlackDB();
	void closeTrainingDB();
	void closeComputerDB();
	void closeStatisticDB();
	void importStatistics();
	void compactStatistics();
	void importPGNTheory();
	void importPGNWhite();
	void importPGNBlack();
	void importBookTheory();
	void importBookWhite();
	void importBookBlack();
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

