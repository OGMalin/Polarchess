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
		pos.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPPRNBQKBNR w KQkq - 0 1");
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

void QChessGame::getMovelist(QStringList& list)
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
		qs = game[current].board().makeMoveText(m.move(), FIDE).c_str();
		for (i = 0; i < 5; i++)
			qs.replace(piecechar.at(i), piecechartr.at(i));
		list.append(qs);
		current = next;
		++lit;
	}
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