#pragma once
#include "Database.h"
#include "../Common/ChessBoard.h"
#include "../Common/ChessMove.h"
#include <QString>
#include <QVector>

class Training
{
private:
	Database* Base[2];
public:
	Training();
	~Training();
	void SetDatabase(int color, Database* base);
	void create(ChessBoard*cb, int color=-1);
};