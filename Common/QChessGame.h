#pragma once

#include <QObject>
#include <QString>
#include <QVector>
#include <QStringList>
#include "../Common/ChessBoard.h"
#include "../Common/ChessMove.h"
#include "../Common/MoveList.h"

class QChessMove
{
public:
	ChessMove _move;
	QString _comment;
	int _second;
	int _next;

	QChessMove() { _next = -1; _move.clear(); _second = -1; };
	ChessMove  move() const{ return _move; };
	QString comment() const { return _comment; };
	int second() const { return _second; };
	int nextposition() { return _next; };
	bool isEmpty() { return _move.empty(); };
	void move(const ChessMove& m) { _move = m; };
	void comment(const QString& s) { _comment = s; };
	void second(const int n) { _second=n; };
	void nextposition(const int n) { _next = n; };
};

#define QChessMoveList QList<QChessMove>

class QChessPosition
{
public:
	ChessBoard _board;
	QString _comment;
	QChessMoveList _moves;
	int _enginescore;
	QString _enginepv;
	ChessBoard board() const { return _board; };
	QString comment() const { return _comment; };
	int enginescore() { return _enginescore; };
	QString enginepv()const { return _enginepv; };
	QChessMoveList moves()const { return _moves; };
	// Find move that give nextpositionindex. Return emptymove if not exist.
	const QChessMove move(int nextpositionindex);
	void board(const ChessBoard& cb) { _board = cb; };
	void comment(const QString& s) { _comment = s; };
	void enginescore(const int n) { _enginescore = n; };
	void enginepv(const QString& s) { _enginepv = s; };
	void addMove(const QChessMove& m) { _moves.push_back(m); };
	void setFen(const QString& fen) { _board.setFen(fen.toLatin1()); };
	void setStartposition() { _board.setStartposition(); };
};

class QChessGame : public QObject
{
	Q_OBJECT

private:
	QVector<QChessPosition> game;
	QList<int> line;
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
	QString _analysisengine;
	QString _annotator;
public:
	QChessGame(QObject *parent=0);
	~QChessGame();
	void clear();
	void newGame(const QString& fen=QString());
	const QChessPosition getPosition();
	const QChessPosition getStartPosition();
	void getMovelist(QStringList& list, int type=FIDE);
	MoveList movelist();
	bool doMove(const QChessMove& move);
	bool doMove(const ChessMove& move);
	typeColor toMove();
	void gotoMove(int ply);
	void addComment(QString& comment, int index = -1);
	int moveCount(int color);
	// Check if last position a 3 fold position
	bool is3fold();
	bool is50move();

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
	void analysisengine(QString& s) { _analysisengine = s; };
	void annotator(QString& s) { _annotator = s; };

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
	const QString analysisengine() { return _analysisengine; };
	const QString annotator() { return _annotator; };
};
