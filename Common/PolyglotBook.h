#pragma once

#include <string>
#include <vector>
#include <map>
#include "../Common/ChessMove.h"
#include "../Common/ChessBoard.h"
#include "../Common/defs.h"

struct PolyglotDataEntry
{
	unsigned short move;
	unsigned short weight;
	unsigned int learn;
	PolyglotDataEntry() { move = weight = learn = 0; };
};

class PolyglotBook
{
public:
	bool isOpen;
	std::string filename;
	PolyglotBook();
	PolyglotBook(std::string& bookfile);
	virtual ~PolyglotBook();
	virtual bool open(std::string& bookfile);
	void close();
	void save();
	void get(ChessBoard& board, std::vector<PolyglotDataEntry>&);
	ChessMove move(ChessBoard&, unsigned short);
	HASHKEY getKey(ChessBoard&);
	void add(ChessBoard&, ChessMove&, unsigned short weight, unsigned int learn);
	bool exist(ChessBoard&);
	ChessMove select(ChessBoard&, std::vector<PolyglotDataEntry>&, bool best);
	int moves();
	int positions();
private:
	bool isDirty;
	std::map<HASHKEY, std::vector<PolyglotDataEntry>> book;
	void add(HASHKEY, PolyglotDataEntry&);
};

