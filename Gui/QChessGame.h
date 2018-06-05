#pragma once

#include <QObject>
#include <QString>
#include <QVector>
#include "../Common/ChessBoard.h"
#include "../Common/ChessMove.h"

struct QChessMove
{
	ChessMove move;
	QString comment;
	int next;
};

struct QChessPosition
{
	ChessBoard board;
	QString comment;
	QList<QChessMove> moves;
};

class QChessGame : public QObject
{
	Q_OBJECT

private:
	QVector<QChessPosition> game;
	QList<int> line;
	int currentPosition;
	QString _site;
	QString _event;
	QString _round;
	QString _date;
	QString _white;
	QString _black;
	QString _result;
	QString _whiteelo;
	QString _blackelo;
	QString _whitetimecontrol;
	QString _blacktimecontrol;
	QString _rated; //Blank, BULLET, BLITZ, RAPID, CLASSICAL
	QString _eco;
public:
	QChessGame(QObject *parent=0);
	~QChessGame();
	void clear();
	void setStartposition(QString& fen);
	bool doMove(QChessMove& move);
	bool doMove(ChessMove& move);
	void site(QString& s) { _site = s; };
	void event(QString& s) { _event = s; };
	void round(QString& s) { _round = s; };
	void white(QString& s) { _white = s; };
	void black(QString& s) { _black = s; };
	void date(QString& s) { _date = s; };
	void result(QString& s) { _result = s; };
	void whiteelo(QString& s) { _whiteelo = s; };
	void blackelo(QString& s) { _blackelo = s; };
	void whitetimecontrol(QString& s) { _whitetimecontrol = s; };
	void blacktimecontrol(QString& s) { _blacktimecontrol = s; };
	void timecontrol(QString& s) { _whitetimecontrol=_blacktimecontrol = s; };
	void rated(QString& s) { _rated = s; };
	void eco(QString& s) { _eco = s; };

	const QString site() { return _site; };
	const QString event() { return _event; };
	const QString round() { return _round; };
	const QString white() { return _white; };
	const QString black() { return _black; };
	const QString date() { return _date; };
	const QString result() { return _result; };
	const QString whiteelo() { return _whiteelo; };
	const QString blackelo() { return _blackelo; };
	const QString whitetimecontrol() { return _whitetimecontrol; };
	const QString blacktimecontrol() { return _blacktimecontrol; };
	const QString timecontrol() { return _whitetimecontrol; };
	const QString rated() { return _rated; };
	const QString eco() { return _eco; };

	const QChessPosition getPosition();
	void getMovelist(QList<QString>& list);
};
