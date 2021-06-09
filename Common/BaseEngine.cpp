#include "../Common/BaseEngine.h"
#include "../Common/Utility.h"
#include <QDebug>
#include <QApplication>
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
	unload();
	if (process)
	{
		process->disconnect();
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
	connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), SLOT(slotFinished()));
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
	HANDLE h = OpenProcess(PROCESS_SET_INFORMATION, TRUE, process->processId());
	SetPriorityClass(h, BELOW_NORMAL_PRIORITY_CLASS);
	CloseHandle(h);
	return true;
}

void BaseEngine::unload()
{
	if (!process)
		return;
	disconnect(process, nullptr, nullptr, nullptr);
	QApplication::processEvents();
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
	while (process && (process->state() != QProcess::NotRunning) && process->canReadLine())
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

void BaseEngine::slotFinished()
{
	process->disconnect();
	emit engineStoped();
}

EngineOption BaseEngine::getOption(QString& name)
{
	int i;
	for (i = 0; i < engineOption.size(); i++)
	{
		if (engineOption[i].name == name)
			return engineOption[i];
	}
	return EngineOption();
}