#pragma once
/*
	Database format
	
	fen			text
		position as fenstring rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR W kqKQ -

	comment		text
		Text comment to the position

	eval		text
		Human evaluation

	attempt		text (used for training)
		Attempt in this position

	score		text (used for training)
		Successfull guesses

	movelist	text
		move1|comment;move2|comment

	Attemt and score are saved on the position where the move are listet.
	Score and attempt for the line are saved on the last position in this line (opponent to move).

*/
#include <QObject>
#include <QSqlDatabase>
#include <QString>
#include <QVector>
#include <QByteArray>
#include <QDataStream>
#include "../Common/ChessMove.h"
#include "../Common/ChessBoard.h"
#include "Path.h"

struct TrainingDBEntry;

struct BookDBMove
{
	ChessMove move;
	QString comment;
	BookDBMove() { clear(); };
	void clear() { move.clear(); comment.clear(); };
};

struct BookDBEntry
{
	ChessBoard board;
	int eval;
	QString comment;
	int attempt;
	int score;
	QVector<BookDBMove> movelist;
	bool dirty;
	BookDBEntry() { clear();  dirty = false; };
	void clear() { board.clear(); comment.clear(); movelist.clear(); eval = attempt = score = 0; dirty = false; };
	bool moveExist(const ChessMove& move);
	void deleteMove(const ChessMove&);
	void setFirst(const ChessMove&);
	void updateMove(BookDBMove&bm, bool mergemove=false);

	void convertToMoveList(QVector<BookDBMove>&, const QString&);
	void convertFromMoveList(const QVector<BookDBMove>&, QString&);

	void merge(BookDBEntry& bde);

	friend bool operator<(const BookDBEntry& b1, const BookDBEntry& b2) { return b1.board < b2.board; };
	friend bool operator==(const BookDBEntry& b1, const BookDBEntry& b2) { return b1.board == b2.board; };
};

struct BookDBInfo
{
	QString db;
	QString version;
	int type;
};

class Database : public QObject
{
	Q_OBJECT

public:
	BookDBInfo bdi;
	Database(QString name, QObject *parent=0);
	~Database();
	// dbtype: 0=general, 1=White repertoire, 2=Black repertoire
	bool create(const QString& path, int dbtype);
	bool open(const QString& path);
	void close();
	bool add(BookDBEntry& data);
	BookDBEntry find(ChessBoard&);
	bool isOpen() { return opened; };
	BookDBInfo bookInfo();
	void getTrainingPosition(QVector<BookDBEntry>&);
	void clearAllTrainingData();
	void importBase(Database* iBase);
	//void addTrainingLines(QVector<TrainingLine>& tlines);
	//void deleteTrainingLines();
	//bool getTrainingLines(QVector<TrainingLine>& lines);
	void updateTrainingScore(TrainingDBEntry*);

	// Get db path
	QString getPath();
private:
	bool opened;
//	QSqlDatabase bookdb;
	QString dbname;
	bool exist(ChessBoard& board);
};
