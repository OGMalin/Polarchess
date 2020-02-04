#pragma once

#include <QDialog>
#include <QString>
#include <QLineEdit>
#include "Database.h"
#include "../Common/Pgn.h"
#include "../Common/ChessGame.h"

struct ExportMove
{
	ChessMove move;
	QString moveComment;
	QString comment;
};

class ExportPgnDialog : public QDialog
{
	Q_OBJECT

public:
	ExportPgnDialog(QWidget *parent);
	void exportPgnFile(QWidget* parent, Database* db);

public slots:
	void saveFile();

private:
	QVector<ExportMove> moves;
	QLineEdit* filename;
	void walkThrough(ChessBoard& cur, int ply, QVector<BookDBEntry>& pos, Pgn& pgn);
};

