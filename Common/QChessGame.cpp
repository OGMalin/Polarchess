#include "QChessGame.h"
#include <QTextStream>

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
	site.clear();
	event.clear();
	white.clear();
	black.clear();
	result=0;
	whiteelo=0;
	blackelo=0;
	round.clear();
	date.setDate(QDate::currentDate().year(), QDate::currentDate().month(), QDate::currentDate().day());
	whiteclock=0;
	blackclock=0;
	timecontrol.clear();
	termination.clear();
	rated=0;
	game.clear();
}

void QChessGame::newGame(const QString& fen)
{
	clear();
	QChessPosition pos;
	if (fen.isEmpty())
		pos.board.setStartposition();
	else
		pos.setFen(fen);
	game.push_back(pos);
}

bool QChessGame::doMove(ChessMove& move, int sec)
{
	QChessMove m;
	m.move=move;
	m.second = sec;
	QChessPosition pos;
	pos.board = game[game.size() - 1].board;
	if (!pos.board.isLegal(move))
		return false;
	game[game.size() - 1].move = m;
	pos.board.doMove(m.move, false);
	game.push_back(pos);
	return true;
}

//bool QChessGame::doMove(const QChessMove& move)
//{
//	ChessBoard cb;
//	int index = line.back();
//	cb = game[index].board();
//	QChessMove qm = move;
//	ChessMove m = qm.move();
//	if (!cb.doMove(m, true))
//		return false;
//	QChessPosition pos;
//	pos.board(cb);
//	qm.nextposition(game.size());
//	game.push_back(pos);
//	game[index].addMove(qm);
//	line.push_back(qm.nextposition());
//	return true;
//}

QChessPosition QChessGame::getPosition(int ply)
{
	if (game.size() == 0)
		return QChessPosition();
	if (ply >= game.size())
		ply = game.size() - 1;
	if (ply == -1)
		ply= game.size() - 1;

	return game[ply];
}

void QChessGame::getMovelist(QStringList& list, int type)
{
	QString piecechartr(tr("NBRQK"));
	QString piecechar("NBRQK");
	if (game.size() == 0)
		return;
	QChessMove m;
	QString qs;
	int i,j;
	for (i=0;i<game.size();i++)
	{
		m=game[i].move;
		if (m.move.empty())
			break;
		qs = game[i].board.makeMoveText(m.move, type).c_str();
		for (j = 0; j < 5; j++)
			qs.replace(piecechar.at(j), piecechartr.at(j));
		list.append(qs);
	}
}

MoveList QChessGame::movelist()
{
	MoveList ml;
	QChessMove m;
	int i;
	if (game.size() == 0)
		return ml;
	for (i=0;i<game.size();i++)
	{
		if (game[i].move.move.empty())
			break;

		ml.push_back(game[i].move.move);
	}
	return ml;
}

bool QChessGame::is3fold()
{
	int count = 0;
	if (game.size() == 0)
		return false;
	ChessBoard cb = game[game.size()-1].board;
	int i;
	for (i=0;i<game.size();i++)
	{
		if (game[i].board == cb)
			++count;
	}
	if (count >= 3)
		return true;
	return false;
}

bool QChessGame::is50move()
{
	int count = 0;
	if (game.size() == 0)
		return false;
	ChessMove m;
	int i;
	for (i=0;i<game.size();i++)
	{
		if (game[i].move.move.empty())
			break;
		m = game[i].move.move;
		if ((m.moveType == PAWNMOVE) || (m.moveType == CASTLE) || (m.moveType == CAPTURE))
			count = 0;
		else
			++count;
	}
	if (count >= 100)
		return true;
	return false;
}

//void QChessGame::addComment(QString& comment, int index)
//{
//	if (line.size() == 0)
//		return;
//	if (index == -1)
//		index = line.back();
//	game[index].comment(comment);
//}

int QChessGame::moveCount(int color)
{
	int mc;
	if (!game.size())
		return 0;
	if (game[0].board.toMove == WHITE)
	{
		if (color==WHITE)
			mc = game.size() / 2;
		else
			mc = (game.size()-1) / 2;
	}
	else
	{
		if (color == BLACK)
			mc = game.size() / 2;
		else
			mc = (game.size() - 1) / 2;
	}
	return mc;
}

//void QChessGame::gotoMove(int ply)
//{
//	int i;
//	QList<int> newline;
//	for (i = 0; i < ply+1; i++)
//	{
//		if (i<line.size())
//			newline.push_back(line[i]);
//	}
//	if (!newline.isEmpty())
//	{
////		newline.back();
//		line = newline;
//	}
//}
//
void QChessGame::getPgn(QString& qs, bool useTime)
{
	int i, j;
	qs.clear();
	QChessMove m;
	QTextStream(&qs) << "[Event \"" << (event.isEmpty() ? "?" : event) << "\"\n";
	QTextStream(&qs) << "[Site \"" << (site.isEmpty() ? "?" : site) << "\"\n";
	QTextStream(&qs) << "[Date \"" << (date.isValid() ? "????.??.??" : date.toString("YYYY.MM.dd")) << "\"\n";
	QTextStream(&qs) << "[Round \"" << (round.isEmpty() ? "?" : round) << "\"\n";
	QTextStream(&qs) << "[White \"" << (white.isEmpty() ? "?" : white) << "\"\n";
	QTextStream(&qs) << "[Black \"" << (black.isEmpty() ? "?" : black) << "\"\n";
	QTextStream(&qs) << "[Result \"" << resultToString() << "\"\n";
	if (!timecontrol.isEmpty())
		QTextStream(&qs) << "[Timecontrol \"" << timecontrol << "\"\n";

	for (i = 0; i < game.size(); i++)
	{
		m = game[i].move;
		if (m.move.empty())
			break;
		QTextStream(&qs) << game[i].board.makeMoveText(m.move, SAN).c_str() << " ";
		if (m.second > 0)
			QTextStream(&qs) << "[%clk " << clockToString(m.second) << "] ";
	}

	QTextStream(&qs) << resultToString() << "\"\n";


}

QString QChessGame::resultToString()
{
	QString qs;
	switch (result)
	{
	case 1: qs = "1-0";
	case 2: qs = "1/2-1/2";
	case 3: qs = "0-1";
	}
	return qs;
}

QString QChessGame::clockToString(int sec)
{
	int h = sec / 3600;
	int m = (sec % 3600) / 60;
	int s = (sec % 60);
	QString qs;
	QTextStream(&qs) << h << ":" << m << ":" << s;
	return qs;
}