#include "QChessGame.h"

QChessGame::QChessGame(QObject *parent)
	: QObject(parent)
{
	currentPosition = -1;
}

QChessGame::~QChessGame()
{
}

void QChessGame::clear()
{
	_site.clear();
	_event.clear();
	_white.clear();
	_black.clear();
	_result.clear();
	_whiteelo.clear();
	_blackelo.clear();
	_round.clear();
	_date.clear();
	_whitetimecontrol.clear();
	_blacktimecontrol.clear();
	_rated.clear();
}

void QChessGame::setStartposition(QString& fen)
{
	game.clear();
	line.clear();
	QChessPosition pos;
	pos.board.setFen(fen.toLatin1());
	game.push_back(pos);
	currentPosition = 0;
	line.push_back(currentPosition);
}

bool QChessGame::doMove(ChessMove& move)
{
	QChessMove m;
	m.move = move;
	return doMove(m);
}

bool QChessGame::doMove(QChessMove& move)
{
	QChessPosition pos;
	if (currentPosition < 0)
		return false;
	pos.board = game[currentPosition].board;
	if (!pos.board.doMove(move.move, true))
		return false;
	game.push_back(pos);
	move.next = game.size();
	game[currentPosition].moves.push_back(move);
	currentPosition = move.next;
	line.push_back(currentPosition);
	return true;
}

