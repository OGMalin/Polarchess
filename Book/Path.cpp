#include "Path.h"
#include <QString>
const char* STARTFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPPRNBQKBNR w KQkq - 0 1";

Path::Path()
{
	clear();
}

Path::~Path()
{

}

void Path::clear()
{
	moves.clear();
	current = 0;
}

ChessBoard Path::getStartPosition()
{
	if (moves.empty())
	{
		ChessBoard b;
		b.setFen(STARTFEN);
		return b;
	}
	return moves[0].board;
}

ChessBoard Path::getPosition()
{
	ChessBoard b;
	int size = moves.size();
	if (current >= size)
		current = size - 1;
	if (current == 0)
		return getStartPosition();
	return moves[current].board;
}

typeColor Path::toMove()
{
	if (moves.empty())
		return WHITE;
	if (current >= moves.size())
		current = moves.size() - 1;
	return moves[current].board.toMove;
}

bool Path::add(ChessMove& move)
{
	int size = moves.size();
	if (current >= size)
		current = size - 1;
	PathEntry p;
	if (size == 0)
		p.board.setFen(STARTFEN);
	else
		p = moves[current];

	if (move == p.move)
	{
		current++;
		return true;
	}

	if (!p.board.isLegal(move))
		return false;

	p.move = move;
	moves[current] = p;


	p.board.doMove(p.move, false);

	moves.remove(current + 1, size - current - 1);
	moves.push_back(p);
	++current;
	return true;
}

int Path::size()
{
	return moves.size();
}

PathEntry Path::getEntry(int n)
{
	PathEntry p;
	int size = moves.size();
	if (size == 0)
	{
		p.board.setFen(STARTFEN);
		return p;
	}

	if (n < moves.size())
	{
		return moves[n];
	}
	p.board=getPosition();
	return p;
}

void Path::setLength(int ply)
{
	int size = moves.size();

	if (ply > size)
		return;

	if (ply <= 0)
	{
		clear();
		return;
	}

	moves.remove(ply,size-ply);


}

void Path::getMoveList(QStringList& ml)
{
	QString s;
	char sz[16];
	ml.clear();
	for (int i = 0; i < moves.size(); i++)
	{
		s=moves[i].board.makeMoveText(moves[i].move,sz,16,FIDE);
		ml.append(s);
	}
}

void Path::setCurrent(int i)
{
	if (i >= moves.size())
		current = moves.size() - 1;
	else
		current = i;
}