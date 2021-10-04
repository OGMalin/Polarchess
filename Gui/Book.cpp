#include "Book.h"
#include <QRandomGenerator>
#include <QStandardPaths>

Book::Book()
	: PolyglotBook()
{

}

Book::~Book()
{
	close();
}

bool Book::open(QString& bookname)
{
	QString path = QStandardPaths::locate(QStandardPaths::DocumentsLocation, "/Polarchess", QStandardPaths::LocateDirectory);
	path += "/" + bookname + ".bin";
	return PolyglotBook::open(path);
}

ChessMove Book::getMove(ChessBoard& cb)
{
	ChessMove m;
	int v, i, j;
	QVector<PolyglotDataEntry> moves;
	get(cb, moves);
	if (moves.size())
	{
		v = 0;
		for (i = 0; i < moves.size(); i++)
			v += moves[i].weight;
		v = QRandomGenerator::global()->bounded(0, v);
		j = 0;
		for (i = 0; i < moves.size(); i++)
		{
			m = move(cb, moves[i].move);
			if ((j + moves[i].weight) >= v)
				return m;
		}
	}
	return m;
}