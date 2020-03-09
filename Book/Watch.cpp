#include "Watch.h"
#include <QHBoxLayout>
#include <QMenu>
#include <QAction>

Watch::Watch(QWidget* parent, bool enableContextMenu)
	:QWidget(parent)
{
	QHBoxLayout* hbox = new QHBoxLayout;
	text = new QLabel("00:00:00");
	if (enableContextMenu)
		setContextMenuPolicy(Qt::CustomContextMenu);
	hbox->addWidget(text);
	setLayout(hbox);
	running = 0;
	time = 0;
	connect(&timer, SIGNAL(timeout()), this, SLOT(updateTime()));
	if (enableContextMenu)
		connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));
}

void Watch::updateTime()
{
	++time;
	char sz[16];
	int h, m, s;
	h = time / 3600;
	m = (time - h * 3600) / 60;
	s = time - h * 3600 - m * 60;
	sprintf_s(sz, 16, "%02d:%02d:%02d", h, m, s);
	text->setText(sz);
}

void Watch::start()
{
	if (running == 0)
		time = 0;
	timer.start(1000);
	running = 2;
}

void Watch::stop()
{
	timer.stop();
	running = 0;
	text->setText("00:00:00");
}

void Watch::pause(bool mark)
{
	timer.stop();
	running = 1;
	if (!mark)
		return;
	QString qs = "* " + text->text();
	text->setText(qs);
}

void Watch::restart()
{
	time = 0;
	timer.start(1000);
	running = 2;
}

void Watch::showContextMenu(const QPoint& pos)
{
	QMenu* contextMenu = new QMenu(this);
	QAction* startAct = contextMenu->addAction(QString("Start"), this, SLOT(start()));
	QAction* stopAct = contextMenu->addAction(QString("Stopp"), this, SLOT(stop()));
	QAction* pauseAct = contextMenu->addAction(QString("Pause"), this, SLOT(pause()));

	switch (running)
	{
	case 0:
		startAct->setDisabled(false);
		stopAct->setDisabled(true);
		pauseAct->setDisabled(true);
		break;
	case 1:
		startAct->setDisabled(false);
		stopAct->setDisabled(false);
		pauseAct->setDisabled(true);
		break;
	case 2:
		startAct->setDisabled(true);
		stopAct->setDisabled(false);
		pauseAct->setDisabled(false);
		break;
	}
	
	contextMenu->exec(mapToGlobal(pos));
}

