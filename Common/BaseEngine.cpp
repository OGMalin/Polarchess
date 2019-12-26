#include "../Common/BaseEngine.h"
#include "../Common/Utility.h"
#include <QDebug>
#include <windows.h>

using namespace std;

BaseEngine::BaseEngine(QObject*parent)
	:QObject(parent)
{
	process = NULL;
	searchtype = NO_SEARCH;
}

BaseEngine::~BaseEngine()
{
	if (process)
	{
		process->waitForFinished();
		delete process;
		process = NULL;
	}
}

bool BaseEngine::load(QString& path)
{
	if (path.isEmpty())
		return false;
	process = new QProcess(this);
	if (!process)
		return false;

	enginePath = path;
	connect(process, SIGNAL(started()), SLOT(slotStarted()));
	connect(process, SIGNAL(finnished()), SLOT(slotFinnished()));
	connect(process, SIGNAL(readyReadStandardOutput()), SLOT(slotReadyReadStandardOutput()));

	process->setReadChannel(QProcess::StandardOutput);

	QString fp;
	int i = path.lastIndexOf("\\");
	int j = path.lastIndexOf("/");
	i = max(i, j);
	if (i > 0)
	{
		fp = path.left(i);
		process->setWorkingDirectory(fp);
	}
	process->setProcessChannelMode(QProcess::MergedChannels);
	process->start(path);
	return true;
}

void BaseEngine::unload()
{
	if (!process)
		return;
	disconnect(process);
	int i = 0;
	process->waitForFinished();
	while (process->state()!=QProcess::NotRunning)
	{
		if (i > 100)
			process->kill();
		_sleep(100);
		++i;
	}
	delete process;
	process = NULL;
}

void BaseEngine::slotReadyReadStandardOutput()
{
	char sz[1024];
	string input;
	int res;
	__int64 readlen;
	while (process && process->canReadLine())
	{
		readlen = process->readLine(sz, 1024);
		if (readlen)
		{
			qDebug(sz);
			input = trim(sz);
			fromEngine(input);
		}
	}
}

void BaseEngine::write(QString& qs)
{
	if (!process)
		return;
	qDebug(qs.toLatin1());
	process->write(qs.toLatin1());
	process->write("\n");
}

void BaseEngine::init(QString&key, QString&val)
{
	initOptions.insertMulti(key, val);
}

void BaseEngine::slotFinnished()
{
	emit engineStoped();
}