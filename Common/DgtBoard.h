#pragma once
#include <QDialog>
#include <QWidget>

// Interfacing with a Dgt electronic chess board.
class DgtBoard : public QDialog
{
	Q_OBJECT
public slots:

signals:
	// Send last status.
	// 0 = Not connected
	// 1 = Connected but not in sync
	// 2 = Connected and in sync
	void dgtStatus(int);
public:
	DgtBoard(QWidget* parent = 0);
	~DgtBoard();
	int status();
private:
	int _status;
};