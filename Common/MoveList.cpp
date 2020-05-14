#include <cstdlib>
#include "../Common/MoveList.h"

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
	if (_list[m1].score< _list[m2].score)
		return 1;
	if (_list[m1].score> _list[m2].score)
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
	_size = 0; 
}

int MoveList::begin()
{ 
	return 0; 
}

int MoveList::end()
{ 
	return _size; 
}

bool MoveList::empty()
{ 
	return (_size == 0);
}

void MoveList::copy(MoveList& ml)
{
	for (int i = 0; i<ml.size(); i++)
		_list[i] = ml._list[i];
	_size = ml.size();
}

ChessMove& MoveList::at(int n)
{
	if (n<_size)
		return _list[n];
	return nomove;
}

void MoveList::push_back(const ChessMove& m)
{
	if (_size<MOVELISTSIZE)
	{
		_list[_size] = m;
		++_size;
	}
}

void MoveList::push_back(const ChessMove& m, int score)
{
	if (_size<MOVELISTSIZE)
	{
		_list[_size] = m;
		_list[_size].score = score;
		++_size;
	}
}

void MoveList::push_front(const ChessMove& m)
{
	int i = _size;
	if (i >= MOVELISTSIZE)
		i = MOVELISTSIZE - 1;
	while (i>0)
	{
		_list[i] = _list[i - 1];
		--i;
	}
	_list[0] = m;
	_size;
	if (_size>MOVELISTSIZE)
		_size = MOVELISTSIZE;
};

void MoveList::push_front(const ChessMove& m, int score)
{
	int i = _size;
	if (i >= MOVELISTSIZE)
		i = MOVELISTSIZE - 1;
	while (i>0)
	{
		_list[i] = _list[i - 1];
		--i;
	}
	_list[0] = m;
	_list[0].score = score;
	++_size;
	if (_size>MOVELISTSIZE)
		_size = MOVELISTSIZE;
};

void MoveList::pop_back()
{
	--_size;
	if (_size < 0)
		_size = 0;

}
int MoveList::find(const ChessMove& m)
{
	int i;
	for (i = 0; i<_size; i++)
		if (_list[i] == m)
			return i;
	return _size;
}

	// Erase n'th element (n=0 is first element)
	// Count is the number of entries to remove, if this is -1
	// The rest of the moves is deleted
void MoveList::erase(int n, int count)
{
	if (n<0)
		n = 0;
	if (n >= _size)
		return;
	if (count<0)
	{
		_size = n;
		return;
	}
	int i = n;
	while ((i + count)<_size)
	{
		_list[i] = _list[i + count];
		++i;
	}
	_size -= count;
	if (_size<0)
		_size = 0;
}

	// Assignment
MoveList& MoveList::operator=(const MoveList& ml)
{
	int i;
	int s = ml.size();
	for (i = 0; i<ml.size(); i++)
		_list[i] = ml._list[i];
	_size = ml.size();
	return *this;
}

void MoveList::swap(int m1, int m2)
{
	ChessMove m;
	if (m1 == m2)
		return;
	m = _list[m1];
	_list[m1] = _list[m2];
	_list[m2] = m;
}

void MoveList::sort(int start, int end)
{
	if (end < 0)
		end = _size - 1;

	bubblesort(start, end);
}

void MoveList::bubblesort(int start, int end)
{
	if ((end - start)<2)
	{
		if (start >= end)
			return;
		if (_list[end].score> _list[start].score)
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
			if (_list[i + 1].score> _list[i].score)
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
	ChessMove m = _list[(first + last) / 2];

	// 5 1 6 6 3 5 7
	//       m
	swap((first + last) / 2, last);
	// 5 1 6 7 3 5 6

	// Loop through the list
	while (first<last)
	{
		// Find the first move that should be sorted after the sample move
		while ((compareMove(&_list[first], &m)<1) && (first<last))
			first++;
		// Find the last move that should be sorted before the sample move
		while ((compareMove(&_list[last], &m)>0) && (first<last))
			last--;
		if (first<last)
			swap(first, last);
	}
	_list[end] = _list[last];
	_list[last] = m;
	quicksort(start, first - 1);
	quicksort(last + 1, end);
}

const ChessMove& MoveList::front()
{
	if (_size)
		return _list[0];
	// No move where found
	_list[0].clear();
	return _list[0];
}

const ChessMove& MoveList::back()
{
	if (_size)
		return _list[_size-1];
	// No move where found
	_list[0].clear();
	return _list[0];
}

	// Removes empty moves from the list
void MoveList::trunc()
{
	int i = 0;
	while (i<_size)
	{
		if (_list[i].empty())
		{
			_size--;
			_list[i] = _list[_size];
		}
		else
		{
			i++;
		}
	}
}

bool MoveList::exist(const ChessMove& m)
{
	if (find(m) == _size)
		return false;
	return true;
}

ChessMove& MoveList::next(int movenr)
{
	int i = movenr + 1;
	while (i<_size)
	{
		if (_list[i].score> _list[movenr].score)
			swap(movenr, i);
		++i;
	}
	return _list[movenr];
}


