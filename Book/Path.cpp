#include "Path.h"
#include <QString>

Path::Path()
{
	clear();
}

Path::~Path()
{

}

void Path::clear()
{
	PathEntry pe;
	moves.clear();
	_current = 0;
	pe.board.setStartposition();
	pe.move.clear();
	moves.push_back(pe);
}

ChessBoard Path::getStartPosition()
{
	return moves[0].board;
}

ChessBoard Path::getPosition()
{
	return moves[_current].board;
}

typeColor Path::toMove()
{
	if (moves.empty())
		return WHITE;
	if (_current >= moves.size())
		_current = moves.size() - 1;
	return moves[_current].board.toMove;
}

bool Path::add(ChessMove& move)
{
	PathEntry p;

	if (move.empty())
		return false;

	p = moves[_current];

	if (move == p.move)
	{
		_current++;
		return true;
	}

	if (!p.board.isLegal(move))
		return false;

	moves[_current].move = p.move=move;


	p.board.doMove(move, false);
	p.move.clear();

	++_current;

	if (moves.size()>_current)
		moves.remove(_current,(moves.size()-_current));
	moves.push_back(p);

	return true;
}

int Path::size()
{
	return moves.size();
}

PathEntry Path::getEntry(int n)
{
	PathEntry p;

	if (n < moves.size())
	{
		return moves[n];
	}
	return p;
}

void Path::getMoveList(QStringList& ml, const char* charset, int type)
{
	QString s;
	char sz[16];
	ml.clear();
	for (int i = 0; i < moves.size(); i++)
	{
		if (moves[i].move.empty())
			break;
		if (charset)
			s = moves[i].board.makeMoveText(moves[i].move, sz, 16, charset);
		else
			s = moves[i].board.makeMoveText(moves[i].move, sz, 16, type);
		ml.append(s);
	}
}

void Path::getMoveList(MoveList& ml)
{
	ml.clear();
	for (int i = 0; i < moves.size(); i++)
	{
		if (moves[i].move.empty())
			break;
		if (i >= _current)
			break;
		ml.push_back(moves[i].move);
	}
}

void Path::current(int n)
{
	if (n >= moves.size())
		n = moves.size() - 1;
	if (n < 0)
		n = 0;
	_current = n;
}

int Path::current()
{
	return _current;
}