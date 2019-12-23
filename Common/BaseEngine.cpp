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
		process->waitForFinished(1000);
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

	connect(process, SIGNAL(errorOccurred(QProcess::ProcessError)), SLOT(errorOccurred(QProcess::ProcessError)));
	connect(process, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(finished(int, QProcess::ExitStatus)));
	connect(process, SIGNAL(readyReadStandardOutput()), SLOT(readyReadStandardOutput()));
	connect(process, SIGNAL(started()), SLOT(started()));

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
	process->start(path);
	return true;
}

void BaseEngine::errorOccurred(QProcess::ProcessError error)
{
	char sz[16];
	QString err = tr("Error message: ");
	err.append(itoa(error, sz, 10));
	emit engineMessage(err);
}

void BaseEngine::finished(int exitCode, QProcess::ExitStatus exitStatus)
{

}

void BaseEngine::readyReadStandardOutput()
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

void BaseEngine::started()
{

}

void BaseEngine::write(const char* sz)
{
	qDebug(sz);
	if (!process)
		return;
	process->write(sz);
	process->write("\n");
}

void BaseEngine::init(QString&key, QString&val)
{
	initOptions.insertMulti(key, val);
}