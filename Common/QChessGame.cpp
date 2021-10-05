#include "QChessGame.h"

const QChessMove QChessPosition::move(int nextpos)
{
	QChessMoveList::iterator mit=_moves.begin();
	while (mit != _moves.end())
	{
		if (nextpos == mit->nextposition())
			return *mit;
	}
	return QChessMove();
}

QChessGame::QChessGame(QObject *parent)
	: QObject(parent)
{
	newGame();
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
	_eco.clear();
	_analysisengine.clear();
	_annotator.clear();
	game.clear();
	line.clear();
}

void QChessGame::newGame(const QString& fen)
{
	game.clear();
	line.clear();
	QChessPosition pos;
	if (fen.isEmpty())
		pos.setStartposition();
	else
		pos.setFen(fen);
	game.push_back(pos);
	line.push_back(0);
}

bool QChessGame::doMove(const ChessMove& move)
{
	QChessMove m;
	m.move(move);
	return doMove(m);
}

bool QChessGame::doMove(const QChessMove& move)
{
	if (line.size() == 0)
		return false;
	ChessBoard cb;
	int index = line.back();
	cb = game[index].board();
	QChessMove qm = move;
	ChessMove m = qm.move();
	if (!cb.doMove(m, true))
		return false;
	QChessPosition pos;
	pos.board(cb);
	qm.nextposition(game.size());
	game.push_back(pos);
	game[index].addMove(qm);
	line.push_back(qm.nextposition());
	return true;
}

const QChessPosition QChessGame::getPosition()
{
	if (line.size()==0)
		return QChessPosition();
	return game[line.back()];
}

const QChessPosition QChessGame::getStartPosition()
{
	if (line.size()==0)
		return QChessPosition();
	return game[line.front()];
}

void QChessGame::getMovelist(QStringList& list, int type)
{
	QString piecechartr(tr("NBRQK"));
	QString piecechar("NBRQK");
	if (line.size() == 0)
		return;
	QChessMove m;
	QString qs;
	int i;
	QList<int>::iterator lit=line.begin();
	int current, next;
	current = *lit;
	++lit;
	while (lit != line.end())
	{
		next = *lit;
		m=game[current].move(next);
		if (m.isEmpty())
			break;
		qs = game[current].board().makeMoveText(m.move(), type).c_str();
		for (i = 0; i < 5; i++)
			qs.replace(piecechar.at(i), piecechartr.at(i));
		list.append(qs);
		current = next;
		++lit;
	}
}

MoveList QChessGame::movelist()
{
	MoveList ml;
	QChessPosition pos;
	QChessMove m;
	QList<int>::iterator lit = line.begin();
	while (lit != line.end())
	{
		if (!game[*lit]._moves.size())
			break;
		ml.push_back(game[*lit]._moves[0].move());
		++lit;
	}
	return ml;
}

bool QChessGame::is3fold()
{
	int count = 0;
	if (line.size() == 0)
		return false;
	ChessBoard cb = game[line.back()].board();
	QList<int>::iterator lit = line.begin();
	while (lit != line.end())
	{
		if (game[*lit].board() == cb)
			++count;
		++lit;
	}
	if (count >= 3)
		return true;
	return false;
}

bool QChessGame::is50move()
{
	int count = 0;
	if (line.size() == 0)
		return false;
	ChessMove m;
	QList<int>::iterator lit = line.begin();
	while (lit != line.end())
	{
		m = game[*lit]._moves[0].move();
		if ((m.moveType == PAWNMOVE) || (m.moveType == CASTLE) || (m.moveType == CAPTURE))
			count == 0;
		else
			++count;
		++lit;
	}
	if (count >= 100)
		return true;
	return false;
}

typeColor QChessGame::toMove()
{
	return getPosition().board().toMove;
}

void QChessGame::addComment(QString& comment, int index)
{
	if (line.size() == 0)
		return;
	if (index == -1)
		index = line.back();
	game[index].comment(comment);
}

int QChessGame::moveCount(int color)
{
	if (line.size() == 0)
		return 0;
	int mc;
	if (getStartPosition().board().toMove == WHITE)
	{
		if (color==WHITE)
			mc = line.size() / 2;
		else
			mc = (line.size()-1) / 2;
	}
	else
	{
		if (color == BLACK)
			mc = line.size() / 2;
		else
			mc = (line.size() - 1) / 2;
	}
	return mc;
}

void QChessGame::gotoMove(int ply)
{
	int i;
	QList<int> newline;
	for (i = 0; i < ply+1; i++)
	{
		if (i<line.size())
			newline.push_back(line[i]);
	}
	if (!newline.isEmpty())
	{
//		newline.back();
		line = newline;
	}
}
