#include "../Common/DgtBoard.h"
#include "../Common/BoardWindow.h"
#include <QDialog>
#include <QBoxLayout>
#include <QDialogButtonBox>

DgtBoard::DgtBoard(QWidget* parent)
	: QDialog(parent)
{
	QHBoxLayout* hbox;
	QVBoxLayout* vbox;
	_status = 0;

	vbox = new QVBoxLayout;
	hbox = new QHBoxLayout;
	BoardWindow* board = new BoardWindow();
	vbox->addWidget(board);
	QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
	vbox->addWidget(buttons);
	setLayout(vbox);

	setWindowFlag(Qt::WindowStaysOnTopHint);
	connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
}

DgtBoard::~DgtBoard()
{

}

int DgtBoard::status()
{
	return _status;
}

