#pragma once

#include <QObject>
#include <QString>
#include <QVector>
#include <QStringList>
#include <QDate>
#include "../Common/ChessBoard.h"
#include "../Common/ChessMove.h"
#include "../Common/MoveList.h"

class QChessMove
{
public:
	ChessMove move;
	// Second on the players clock after the move are done.
	int second;

	QChessMove() {move.clear(); second = -1; };
};


class QChessPosition
{
public:
	ChessBoard board;
	QChessMove move;
	void setFen(const QString& fen) { board.setFen(fen.toLatin1()); };
};

class QChessGame : public QObject
{
	Q_OBJECT

private:
public:
	QVector<QChessPosition> game;
	QString site;
	QString event;
	QString round;
	QDate date;
	QString white;
	QString black;
	int result; // 0=no result, 1=Hvit, 2=remis, 3=black win
	int whiteelo;
	int blackelo;
	int whiteclock;
	int blackclock;
	QString timecontrol;
	QString termination;
	int rated; //Unrated=0, BULLET=1, BLITZ=2, RAPID=3, CLASSICAL=4
	QChessGame(QObject *parent=0);
	~QChessGame();
	void clear();
	void newGame(const QString& fen=QString());
	// Get a position, 0=startposition, -1=last position
	QChessPosition getPosition(int ply = -1);
	typeColor toMove() { return getPosition().board.toMove; };
	bool doMove(ChessMove& move, int sec);
	void getMovelist(QStringList& list, int type=FIDE);
	void getPgn(QString&, bool useTime = true);
	MoveList movelist();
	//bool doMove(const QChessMove& move);
	//void gotoMove(int ply);
	//void addComment(QString& comment, int index = -1);
	int moveCount(int color);
	//// Check if last position a 3 fold position
	bool is3fold();
	bool is50move();
	QString resultToString();
	QString clockToString(int sec);
};
