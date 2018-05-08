#pragma once

#include "ChessBoard.h"

#define MAX_DRAWTABLE 1024

class DrawTable
{
	HASHKEY hTable[MAX_DRAWTABLE];
	ChessBoard bTable[MAX_DRAWTABLE];
	int size;
public:
	DrawTable() { clear(); };
	void clear() { size = 0; };
	void add(ChessBoard& cb)
	{
		if (size >= MAX_DRAWTABLE)
			return;
		hTable[size] = cb.hashkey();
		bTable[size] = cb;
		++size;
	};
	bool exist(ChessBoard&cb)
	{
		HASHKEY key = cb.hashkey();
		return exist(cb, key);
	};
	bool exist(ChessBoard& cb, HASHKEY key)
	{
		int it;
		while (it < size)
		{
			if (hTable[it] == key)
				if (bTable[it].compare(cb)==0)
					return true;
		};
		return false;
	};
};