#pragma once

#include "../Common/PolyglotBook.h"
#include "../Common/ChessBoard.h"
#include "../Common/MoveGenerator.h"
#include <QDialog>
#include <QString>
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>
#include <QNetworkAccessManager>
#include <QEventLoop>

class ImportLCProgress : public QDialog
{
	Q_OBJECT
public:
	ImportLCProgress(QWidget* parent);
	QLabel* status;
	QLabel* count;
	bool abort;
public slots:
	void aborting();
};

class ImportLC : public QDialog
{

	Q_OBJECT

public slots:
//	void replyFinnish(QNetworkReply*);
signals:
	void newPosition(ChessBoard&);
public:
	PolyglotBook* book;
	ImportLC(QWidget* parent);
	virtual ~ImportLC();
	void Import(QWidget* parent, PolyglotBook*);
private:
//	QNetworkAccessManager* manager;
//	QNetworkReply* reply;
//	ChessBoard cbtemp;
	ImportLCProgress* progress;
	int count;
	MoveGenerator mgen;
	QLineEdit *link;
	QCheckBox *rating1600, *rating1800, *rating2000, *rating2200, *rating2500;
	QCheckBox *bullet, *blitz, *rapid, *classical;
	QLineEdit *played, *playedp, *playedpfirst;
	QCheckBox* overwright;
	void importTravel(ChessBoard&, QEventLoop&, QNetworkAccessManager&, QString& url, bool first=false);
};