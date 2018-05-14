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
		int it=0;
		while (it < size)
		{
			if (hTable[it] == key)
				if (bTable[it].compare(cb)==0)
					return true;
			++it;
		};
		return false;
	};
};

// Drawtable used under search
class HashDrawTable
{
	HASHKEY hTable[MAX_PLY];
	int size;
public:
	HashDrawTable() {};
	void add(HASHKEY key, int ply)
	{
		hTable[ply] = key;
	};
	bool exist(HASHKEY key, int ply)
	{
		int it=ply;
		while (it>=0)
		{
			if (hTable[it] == key)
				return true;
			--it;
		};
		return false;
	};
};

