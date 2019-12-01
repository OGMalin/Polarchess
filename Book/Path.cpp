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
	if (size == 0)
	{
		b.setFen(STARTFEN);
	}
	else
	{
		b = moves[size - 1].board;
		b.doMove(moves[size - 1].move,false);
	}
	return b;
}

typeColor Path::toMove()
{
	if (moves.empty())
		return WHITE;

	return moves.back().board.toMove;
}

bool Path::add(ChessMove& move)
{
	int size = moves.size();

	PathEntry p;
	if (size == 0)
	{
		p.board.setFen(STARTFEN);
	}
	else
	{
		p.board = moves[size - 1].board;
		p.board.doMove(moves[size - 1].move,false);
	}

	if (!p.board.isLegal(move))
		return false;

	p.move = move;
	moves.push_back(p);
	current = moves.size() - 1;
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