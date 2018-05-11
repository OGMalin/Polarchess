#include <cstdlib>
#include "MoveList.h"

int MoveList::compareMove(const void* m1, const void* m2)
{
	if (((ChessMove*)(m1))->score<((ChessMove*)(m2))->score)
		return 1;
	if (((ChessMove*)(m1))->score>((ChessMove*)(m2))->score)
		return -1;
	return 0;
}

int MoveList::compareMove(int m1, int m2)
{
	if (list[m1].score<list[m2].score)
		return 1;
	if (list[m1].score>list[m2].score)
		return -1;
	return 0;
}

MoveList::MoveList()
{ 
	clear();
}

MoveList::~MoveList()
{
}

void MoveList::clear()
{ 
	size = 0; 
}

int MoveList::begin()
{ 
	return 0; 
}

int MoveList::end()
{ 
	return size; 
}

bool MoveList::empty()
{ 
	return (size == 0);
}

void MoveList::copy(MoveList& ml)
{
	for (int i = 0; i<ml.size; i++)
		list[i] = ml.list[i];
	size = ml.size;
}

ChessMove& MoveList::at(int n)
{
	if (n<size)
		return list[n];
	return nomove;
}

void MoveList::push_back(const ChessMove& m)
{
	if (size<MOVELISTSIZE)
	{
		list[size] = m;
		size++;
	}
}

void MoveList::push_front(const ChessMove& m)
{
	int i = size;
	if (i >= MOVELISTSIZE)
		i = MOVELISTSIZE - 1;
	while (i>0)
	{
		list[i] = list[i - 1];
		--i;
	}
	list[0] = m;
	size++;
	if (size>MOVELISTSIZE)
		size = MOVELISTSIZE;
};

int MoveList::find(const ChessMove& m)
{
	int i;
	for (i = 0; i<size; i++)
		if (list[i] == m)
			return i;
	return size;
}

	// Erase n'th element (n=0 is first element)
	// Count is the number of entries to remove, if this is -1
	// The rest of the moves is deleted
void MoveList::erase(int n, int count)
{
	if (n<0)
		n = 0;
	if (n >= size)
		return;
	if (count<0)
	{
		size = n;
		return;
	}
	int i = n;
	while ((i + count)<size)
	{
		list[i] = list[i + count];
		++i;
	}
	size -= count;
	if (size<0)
		size = 0;
}

	// Assignment
MoveList& MoveList::operator=(const MoveList& ml)
{
	int i;
	for (i = 0; i<ml.size; i++)
		list[i] = ml.list[i];
	size = ml.size;
	return *this;
}

void MoveList::swap(int m1, int m2)
{
	ChessMove m;
	if (m1 == m2)
		return;
	m = list[m1];
	list[m1] = list[m2];
	list[m2] = m;
}

void MoveList::sort(int start, int end)
{
	if (end < 0)
		end = size - 1;

	bubblesort(start, end);
}

void MoveList::bubblesort(int start, int end)
{
	if ((end - start)<2)
	{
		if (start >= end)
			return;
		if (list[end].score>list[start].score)
			swap(start, end);
		return;
	}
	bool unsorted = true;
	int i, last = end;
	while (unsorted)
	{
		unsorted = false;
		for (i = start; i<last; i++)
		{
			if (list[i + 1].score>list[i].score)
			{
				swap(i, i + 1);
				unsorted = true;
			}
		}
		last--;
	}
}

void MoveList::quicksort(int start, int end)
{
	// Run Bubblesearch for short movelists
	if ((end - start)<6)
	{
		bubblesort(start, end);
		return;
	}

	int first = start;
	int last = end;

	// Take a sample move from the midle
	ChessMove m = list[(first + last) / 2];

	// 5 1 6 6 3 5 7
	//       m
	swap((first + last) / 2, last);
	// 5 1 6 7 3 5 6

	// Loop through the list
	while (first<last)
	{
		// Find the first move that should be sorted after the sample move
		while ((compareMove(&list[first], &m)<1) && (first<last))
			first++;
		// Find the last move that should be sorted before the sample move
		while ((compareMove(&list[last], &m)>0) && (first<last))
			last--;
		if (first<last)
			swap(first, last);
	}
	list[end] = list[last];
	list[last] = m;
	quicksort(start, first - 1);
	quicksort(last + 1, end);
}

const ChessMove& MoveList::front()
{
	if (size)
		return list[0];
	// No move where found
	list[0].clear();
	return list[0];
}

const ChessMove& MoveList::back()
{
	if (size)
		return list[size-1];
	// No move where found
	list[0].clear();
	return list[0];
}

	// Removes empty moves from the list
void MoveList::trunc()
{
	int i = 0;
	while (i<size)
	{
		if (list[i].empty())
		{
			size--;
			list[i] = list[size];
		}
		else
		{
			i++;
		}
	}
}

bool MoveList::exist(const ChessMove& m)
{
	if (find(m) == size)
		return false;
	return true;
}

ChessMove& MoveList::next(int movenr)
{
	int i = movenr + 1;
	while (i<size)
	{
		if (list[i].score>list[movenr].score)
			swap(movenr, i);
		++i;
	}
	return list[movenr];
}


