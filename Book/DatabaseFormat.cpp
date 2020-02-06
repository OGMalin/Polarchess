#include "DatabaseFormat.h"
#include <QStringList>
#include <string>

bool TrainingDBEntry::isCorrect(ChessMove& move)
{
	if (moves[current].move == move)
		return true;
	return false;
}

bool TrainingDBEntry::nextMove(ChessMove& move)
{
	if ((current + 2) >= moves.size())
		return false;

	move = moves[current + 1].move;
	current += 2;
	return true;
}

ChessMove TrainingDBEntry::currentMove()
{
	return moves[current].move;
};

int TrainingDBEntry::toMove()
{
	return (current % 2);
}

void TrainingDBEntry::moveList(MoveList& ml)
{
	int i;
	for (i = 0; i < moves.size(); i++)
	{
		if (i >= current)
			break;
		ml.push_back(moves[i].move);
	}
}

void TrainingDBEntry::scoreLine()
{
	int i, sc, at;
	int start = 0;
	sc = at = 999999;
	if (color == BLACK)
		start = 1;
	for (i = start; i < moves.size(); i = i + 2)
	{
		at = __min(at, moves[i].attempt);
		sc = __min(sc, moves[i].score);
	}
	score = at + sc;
}

bool BookDBEntry::moveExist(const ChessMove& move)
{
	QVector<BookDBMove>::iterator it = movelist.begin();
	while (it != movelist.end())
	{
		if (it->move == move)
			return true;
		++it;
	}
	return false;
}

void BookDBEntry::deleteMove(const ChessMove& move)
{
	int i;
	for (i = 0; i < movelist.size(); i++)
	{
		if (movelist[i].move == move)
		{
			movelist.remove(i);
			return;
		}
	}
}

void BookDBEntry::setFirst(const ChessMove& move)
{
	int i;
	for (i = 0; i < movelist.size(); i++)
		if (movelist[i].move == move)
			break;
	if (i == movelist.size()) // Not found
		return;
	if (i == 0) // Allready first
		return;
	BookDBMove bm = movelist[0];
	movelist[0] = movelist[i];
	movelist[i] = bm;
}

void BookDBEntry::convertToMoveList(QVector<BookDBMove>& movelist, const QString& data)
{
	BookDBMove bdm;
	QStringList qmove;
	QStringList qlist = data.split(';');

	movelist.clear();
	for (int i = 0; i < qlist.size(); i++)
	{
		qmove = qlist[i].split('|');
		bdm.clear();
		if (qmove.size() > 0)
			bdm.move = board.getMoveFromText(qmove[0].toStdString());
		if (qmove.size() > 1)
			bdm.comment = qmove[1];
		if (!bdm.move.empty())
			movelist.append(bdm);
	}
}

void BookDBEntry::convertFromMoveList(const QVector<BookDBMove>& movelist, QString& data)
{
	data.clear();
	char sz[16];
	bool first = true;
	QVector<BookDBMove>::ConstIterator it = movelist.begin();
	while (it != movelist.end())
	{
		if (board.isLegal(ChessMove(it->move)))
		{
			if (!first)
				data += ";";
			else
				first = false;
			data += board.makeMoveText(it->move, sz, 16, FIDE);
			data += "|";
			data += it->comment;
		}
		++it;
	}
}

void BookDBEntry::updateMove(BookDBMove& bm, bool mergemove)
{
	for (int i = 0; i < movelist.size(); i++)
	{
		if (bm.move == movelist[i].move)
		{
			if (mergemove)
			{
				if (movelist[i].comment.isEmpty())
					movelist[i].comment = bm.comment;
				return;
			}
			movelist[i] = bm;
			return;
		}
	}
	movelist.append(bm);
}

void BookDBEntry::merge(BookDBEntry& bde)
{
	board = bde.board;
	if (!eval)
		eval = bde.eval;
	if (!bde.comment.isEmpty())
	{
		if (comment != bde.comment)
		{
			if (!comment.isEmpty())
				comment += "\n";
			comment += bde.comment;
		}
	}
	if (!score)
		score = bde.score;
	for (int i = 0; i < bde.movelist.size(); i++)
		updateMove(bde.movelist[i], true);
}

void StatisticsDBEntry::convertToMoveList(QVector<StatisticsDBMove>& movelist, const QString& data, ChessBoard& cb)
{
	StatisticsDBMove sdm;
	QStringList qmove;
	QStringList qlist = data.split(';');

	movelist.clear();
	for (int i = 0; i < qlist.size(); i++)
	{
		qmove = qlist[i].split('|');
		sdm.clear();
		if (qmove.size() > 0)
			sdm.move = cb.getMoveFromText(qmove[0].toStdString());
		if (qmove.size() > 1)
			sdm.whitewin = qmove[1].toInt();
		if (qmove.size() > 2)
			sdm.draw = qmove[2].toInt();
		if (qmove.size() > 3)
			sdm.blackwin = qmove[3].toInt();
		if (qmove.size() > 4)
			sdm.elo = qmove[4].toInt();
		if (qmove.size() > 5)
			sdm.year = qmove[5].toInt();
		if (!sdm.move.empty())
			movelist.append(sdm);
	}
}

void StatisticsDBEntry::convertFromMoveList(const QVector<StatisticsDBMove>& movelist, QString& data, ChessBoard& cb)
{
	data.clear();
	char sz[16];
	bool first = true;
	QVector<StatisticsDBMove>::ConstIterator it = movelist.begin();
	while (it != movelist.end())
	{
		if (cb.isLegal(ChessMove(it->move)))
		{
			if (!first)
				data += ";";
			else
				first = false;
			data += cb.makeMoveText(it->move, sz, 16, FIDE);
			data += "|";
			data += itoa(it->whitewin, sz, 10);
			data += "|";
			data += itoa(it->draw, sz, 10);
			data += "|";
			data += itoa(it->blackwin, sz, 10);
			data += "|";
			data += itoa(it->elo, sz, 10);
			data += "|";
			data += itoa(it->year, sz, 10);
		}
		++it;
	}
}

void StatisticsDBEntry::updateMove(StatisticsDBMove& bm)
{
	for (int i = 0; i < movelist.size(); i++)
	{
		if (bm.move == movelist[i].move)
		{
			movelist[i].whitewin += bm.whitewin;
			movelist[i].draw += bm.draw;
			movelist[i].blackwin += bm.blackwin;
			movelist[i].elo = __max(movelist[i].elo, bm.elo);
			movelist[i].year = __max(movelist[i].year, bm.year);
			return;
		}
	}
	movelist.append(bm);
}

void ComputerDBEntry::convertToEngineList(const QString& data, ChessBoard& cb)
{
	//	engine1 | cp | depth | time | pv; engine2 | cb | depth | time | pv

	QStringList qlist = data.split(';');
	QStringList entry;
	QStringList smoves;
	ComputerDBEngine ce;
	ChessBoard b;
	ChessMove m;
	QStringList::iterator mit;
	enginelist.clear();

	QStringList::iterator it = qlist.begin();
	while (it != qlist.end())
	{
		ce.clear();
		entry = it->split('|');
		if (entry.size() > 0)
			ce.engine = entry[0];
		if (entry.size() > 1)
			ce.cp = entry[1].toInt();
		if (entry.size() > 2)
			ce.depth = entry[2].toInt();
		if (entry.size() > 3)
			ce.time = entry[3].toInt();
		if (entry.size() > 4)
		{
			smoves = entry[4].split(' ');
			mit = smoves.begin();
			b = cb;
			while (mit != smoves.end())
			{
				m = b.getMoveFromText(mit->toStdString());
				ce.pv.push_back(m);
				b.doMove(m, false);
				++mit;
			}
		}
		enginelist.push_back(ce);
		++it;
	}
}

void ComputerDBEntry::convertFromEngineList(QString& data, ChessBoard& cb)
{
	//	engine1 | cp | depth | time | pv; engine2 | cb | depth | time | pv
	data.clear();
	char sz[16];
	std::string s;
	ChessBoard b;
	int mit;
	QVector<ComputerDBEngine>::iterator it = enginelist.begin();
	while (it != enginelist.end())
	{
		if (it != enginelist.begin())
			data += ";";
		data += it->engine;
		data += "|";
		data += itoa(it->cp, sz, 10);
		data += "|";
		data += itoa(it->depth, sz, 10);
		data += "|";
		data += itoa(it->time, sz, 10);
		data += "|";
		b = cb;
		for (mit = 0; mit < it->pv.size(); mit)
		{
			s = b.makeMoveText(it->pv[mit], FIDE);
			if (mit > 0)
				data += " ";
			data += s.c_str();
			b.doMove(it->pv[mit], false);
			++mit;
		}
		++it;
	}
}

bool ComputerDBEntry::updateEngine(ComputerDBEngine& ce)
{
	QVector<ComputerDBEngine>::iterator it = enginelist.begin();
	while (it != enginelist.end())
	{
		if (it->engine == ce.engine)
		{
			if ((ce.depth > it->depth) || (ce.time > it->time) && (ce.depth == it->depth))
			{
				*it = ce;
				return true;
			}
			else
			{
				return false;
			}
		}
		++it;
	}
	enginelist.push_back(ce);
	return true;
}

QByteArray CompressedBoard::compress(ChessBoard& cb)
{
	QByteArray data;
	unsigned long long occupied=0;
	unsigned char pieces[32];
	int i;
	int p;
	int next = 0;
	memset(pieces, 0, 32);
	for (i = 63; i > 0; i--)
	{
		if (cb[i] != EMPTY)
			occupied |= 1;
		occupied = occupied << 1;
	}
	if (cb[0] != EMPTY)
		occupied |= 1;
	for (i = 0; i < 64; i++)
	{
		if (cb[i] != EMPTY)
		{
			switch (cb[i])
			{
			case whitepawn:
				if (SQUARE64(cb.enPassant) == (i - 8))
					p = epPawn;
				else
					p = WhitePawn;
				pieces[next / 2] |= (next % 2 ? p << 4 : p);
				break;
			case whiteknight:
				pieces[next / 2] |= (next % 2 ? WhiteKnight << 4 : WhiteKnight);
				break;
			case whitebishop:
				pieces[next / 2] |= (next % 2 ? WhiteBishop << 4 : WhiteBishop);
				break;
			case whiterook:
				if (((cb.castle&whitekingsidecastle) && (i == H1)) || ((cb.castle&whitequeensidecastle) && (i == A1)))
					p = CastleRook;
				else
					p = WhiteRook;
				pieces[next / 2] |= (next % 2 ? p << 4 : p);
				break;
			case whitequeen:
				pieces[next / 2] |= (next % 2 ? WhiteQueen << 4 : WhiteQueen);
				break;
			case whiteking:
				if (cb.toMove == WHITE)
					p = tomoveKing;
				else
					p = WhiteKing;
				pieces[next / 2] |= (next % 2 ? p << 4 : p);
				break;
			case blackpawn:
				if (SQUARE64(cb.enPassant) == (i - 8))
					p = epPawn;
				else
					p = BlackPawn;
				pieces[next / 2] |= (next % 2 ? p << 4 : p);
				break;
			case blackknight:
				pieces[next / 2] |= (next % 2 ? BlackKnight << 4 : BlackKnight);
				break;
			case blackbishop:
				pieces[next / 2] |= (next % 2 ? BlackBishop << 4 : BlackBishop);
				break;
			case blackrook:
				if (((cb.castle&blackkingsidecastle) && (i == H8)) || ((cb.castle&blackqueensidecastle) && (i == A8)))
					p = CastleRook;
				else
					p = BlackRook;
				pieces[next / 2] |= (next % 2 ? p << 4 : p);
				break;
			case blackqueen:
				pieces[next / 2] |= (next % 2 ? BlackQueen << 4 : BlackQueen);
				break;
			case blackking:
				if (cb.toMove == BLACK)
					p = tomoveKing;
				else
					p = BlackKing;
				pieces[next / 2] |= (next % 2 ? p << 4 : p);
				break;
			}
			++next;
		}
		if (next >= 32)
			break;
	}
	data.push_back(occupied&0xff);
	occupied = occupied >> 8;
	data.push_back(occupied & 0xff);
	occupied = occupied >> 8;
	data.push_back(occupied & 0xff);
	occupied = occupied >> 8;
	data.push_back(occupied & 0xff);
	occupied = occupied >> 8;
	data.push_back(occupied & 0xff);
	occupied = occupied >> 8;
	data.push_back(occupied & 0xff);
	occupied = occupied >> 8;
	data.push_back(occupied & 0xff);
	occupied = occupied >> 8;
	data.push_back(occupied & 0xff);
	occupied = occupied >> 8;
	for (i = 0; i < next / 2; i++)
		data.push_back(pieces[i]);
	return data;
}

ChessBoard CompressedBoard::decompress(QByteArray& data)
{
	ChessBoard cb;
	if (data.size() < 8)
		return cb;
	unsigned long long occupied;
	int i;
	occupied = (unsigned char) data[7];
	occupied = occupied << 8;
	occupied += (unsigned char) data[6];
	occupied = occupied << 8;
	occupied += (unsigned char) data[5];
	occupied = occupied << 8;
	occupied += (unsigned char )data[4];
	occupied = occupied << 8;
	occupied += (unsigned char) data[3];
	occupied = occupied << 8;
	occupied += (unsigned char) data[2];
	occupied = occupied << 8;
	occupied += (unsigned char) data[1];
	occupied = occupied << 8;
	occupied += (unsigned char) data[0];
	int next = 0;
	unsigned char p;
	int o;
	int wK, bK, mK;
	wK = bK = mK = -1;
	for (i = 0; i < 64; i++)
	{
		o = occupied & 0x01;
		occupied = occupied >> 1;
		if ((next / 2 + 8) >= data.size())
			break;
		if (o)
		{
			if (next % 2)
				p = (unsigned char) data[(next / 2) + 8] >> 4;
			else
				p = (unsigned char) data[(next / 2) + 8] & 0x0f;
			switch (p)
			{
			case WhitePawn:
				cb[i] = whitepawn;
				break;
			case WhiteKnight:
				cb[i] = whiteknight;
				break;
			case WhiteBishop:
				cb[i] = whitebishop;
				break;
			case WhiteRook:
				cb[i] = whiterook;
				break;
			case WhiteQueen:
				cb[i] = whitequeen;
				break;
			case WhiteKing:
				wK = i;
				break;
			case BlackPawn:
				cb[i] = blackpawn;
				break;
			case BlackKnight:
				cb[i] = blackknight;
				break;
			case BlackBishop:
				cb[i] = blackbishop;
				break;
			case BlackRook:
				cb[i] = blackrook;
				break;
			case BlackQueen:
				cb[i] = blackqueen;
				break;
			case BlackKing:
				bK = i;
				break;
			case CastleRook:
				if (i == A1)
				{
					cb[i] = whiterook;
					cb.castle |= whitequeensidecastle;
				}
				else if (i ==H1)
				{
					cb[i] = whiterook;
					cb.castle |= whitekingsidecastle;
				}
				else if (i == A8)
				{
					cb[i] = blackrook;
					cb.castle |= blackqueensidecastle;
				}
				else if (i == H8)
				{
					cb[i] = blackrook;
					cb.castle |= blackkingsidecastle;
				}
				break;
			case epPawn:
				if ((i >= A4) && (i <= H4))
				{
					cb.enPassant = SQUARE128(i - 8);
					cb[i] = whitepawn;
				}
				else if ((i >= A5) && (i <= H5))
				{
					cb.enPassant = SQUARE128(i - 8);
					cb[i] = blackpawn;
				}
				break;
			case tomoveKing:
				mK = i;
				break;
			}
			++next;
		}
	}
	if (mK < 0)
		return cb; // Wrong data

	if (wK >= 0)
	{
		cb[wK] = whiteking;
		cb[mK] = blackking;
		cb.toMove = BLACK;
	}
	else
	{
		cb[mK] = whiteking;
		cb[bK] = blackking;
		cb.toMove = WHITE;
	}
	return cb;
}
