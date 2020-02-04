#pragma once

#include "../Common/ChessMove.h"
#include "../Common/MoveList.h"
#include "../Common/ChessBoard.h"
#include <QVector>


// Training database
/*
	Database format

	color			text
		The color for the player.

	score	text
		Calculated score from attempt and score for each move

	moves
		move1|attempt|score;move1|attempt|score
*/

struct TrainingDBInfo
{
	QString db;
	QString version;
};

struct TrainingDBMove
{
	ChessMove move;
	int attempt;
	int score;
	void clear() { move.clear(); score = 0; };
};

struct TrainingDBEntry
{
	QVector<TrainingDBMove> moves;
	int rowid;
	int score; // Score for this line made by calculating score and attempt for each move.
	int color;
	int current;
	//	ChessBoard endposition;
	void clear() { moves.clear(); score = 0; color = 0; current = 0; rowid = 0;/* endposition.clear(); */ };
	friend bool operator<(const TrainingDBEntry& t1, const TrainingDBEntry& t2) { return t1.score < t2.score; };
	bool isCorrect(ChessMove& move);
	bool nextMove(ChessMove& move);
	ChessMove currentMove();
	QString MovesToString();
	void MovesFromString(const QString&);
	int toMove();
	void moveList(MoveList&);
	void scoreLine();
	void incAttempt() { if (!moves[current].attempt)++moves[current].attempt; };
	void incScore() { if (!moves[current].score)++moves[current].score; };
	void decScore() { if (!moves[current].score)--moves[current].score; };
	void clearScore() { for (int i = 0; i < moves.size(); i++)moves[i].score = moves[i].attempt = 0; };
};

struct TrainingStatistics
{
	int loaded;
	int inBase;
};

// Book database
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
	void updateMove(BookDBMove&bm, bool mergemove = false);

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

/*
	Database format

	hash			text
		position as hashkey.

	movelist	text
		move1|win|draw|loss|maxelo|lastyear;move2|win|draw|loss|maxelo|lastyear
*/

struct StatisticsDBMove
{
	ChessMove move;
	int elo;
	int whitewin, draw, blackwin;
	int year;
	StatisticsDBMove() { clear(); };
	void clear() { move.clear(); whitewin = draw = blackwin = elo = year = 0; };
	friend bool operator<(const StatisticsDBMove& m1, const StatisticsDBMove& m2) { return (m2.whitewin + m2.draw + m2.blackwin) < (m1.whitewin + m1.draw + m1.blackwin); };
};

struct StatisticsDBEntry
{
	HASHKEY hash;
	QVector<StatisticsDBMove> movelist;
	StatisticsDBEntry() { clear(); };
	void clear() { hash = 0; movelist.clear(); };
	//	bool moveExist(ChessMove& move);
	void updateMove(StatisticsDBMove&bm);

	void convertToMoveList(QVector<StatisticsDBMove>&, const QString&, ChessBoard&);
	void convertFromMoveList(const QVector<StatisticsDBMove>&, QString&, ChessBoard&);
};

struct StatisticsDBInfo
{
	QString db;
	QString version;
};

/*
	Database format

	hash			text
		position as hashkey.

	enginelist	text
		engine1|cp|depth|time|pv;engine2|cb|depth|time|pv
*/

struct ComputerDBEngine
{
	QString engine;
	int cp;
	int depth;
	int time;
	MoveList pv;
	ComputerDBEngine() { clear(); };
	void clear() { engine.clear(); cp = depth = 0; pv.clear(); };
};

struct ComputerDBEntry
{
	HASHKEY hash;
	QVector<ComputerDBEngine> enginelist;
	ComputerDBEntry() { clear(); };
	void clear() { hash = 0; enginelist.clear(); };
	//	bool engineExist(QString& e);
	bool updateEngine(ComputerDBEngine&);

	void convertToEngineList(const QString&, ChessBoard&);
	void convertFromEngineList(QString&, ChessBoard&);
};

struct ComputerDBInfo
{
	QString db;
	QString version;
};


struct CompressedBoard
{
	/* Format
		64 bit (8 byte) with occupied, bit 0 = a1 bit 1 = a2  ... bit 63 = h8 etc.
		4 bit each piece
		Max size = 192 bit (24 byte)
	*/

	enum { WhitePawn = 0, WhiteKnight, WhiteBishop, WhiteRook, WhiteQueen, WhiteKing, BlackPawn, BlackKnight, BlackBishop, BlackRook, BlackQueen, BlackKing, CastleRook, epPawn, tomoveKing };
	QByteArray compress(ChessBoard&) const;
	ChessBoard decompress(QByteArray&) const;
};

