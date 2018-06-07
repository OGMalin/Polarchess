#include <QProcess>
#include "Engine.h"

Engine::Engine()
{
	process = NULL;
}

Engine::~Engine()
{
	unload();
}

void Engine::setEngine(QString& name, QString& dir)
{
	engineName = name;
	workingDir = dir;
}

bool Engine::load()
{
	if (process)
		return false;

	if (engineName.isEmpty())
		return false;
	
	process = new QProcess(this);

	if (!process)
		return false;

	connect(process, SIGNAL(errorOccurred(QProcess::ProcessError)),  SLOT(slotErrorOccurred(QProcess::ProcessError)));
	connect(process, SIGNAL(finished(int, QProcess::ExitStatus)),  SLOT(slotFinished(int, QProcess::ExitStatus)));
	connect(process, SIGNAL(readyReadStandardOutput()), SLOT(slotReadyStandardOutput()));
	connect(process, SIGNAL(started()), SLOT(slotStarted()));
	//	connect(process, SIGNAL(stateChanged(QProcess::ProcessState newState)), SLOT(slotStateChanged(QProcess::ProcessState newState)));
	//	connect(process, SIGNAL(readyRead()), SLOT(slotReadyRead()));

	process->setReadChannel(QProcess::StandardOutput);

	if (!workingDir.isEmpty())
		process->setWorkingDirectory(workingDir);

	emit engineMessage("Starting engine");
	process->start(engineName);
	return true;
}

void Engine::unload()
{
	if (!process)
		return;
	process->write("quit\n");
	if (process)
		process->waitForFinished(1000);
	delete process;
	process = NULL;
}

const QString Engine::lastError()
{
	return lasterror;
}

void Engine::slotErrorOccurred(QProcess::ProcessError e)
{
	char sz[16];
	QString err = tr("Error message: ");
	err.append(itoa(e, sz, 10));
	emit engineMessage(err);
}

void Engine::slotFinished(int exitCode, QProcess::ExitStatus exitStatus)
{

}

void Engine::slotReadyStandardOutput()
{
	QString msg;

	while (process && process->canReadLine())
	{
		msg = process->readLine().simplified();
		emit engineMessage(msg);
	}
//	QByteArray data = process->readAllStandardOutput();
//	process->readLine().simplified();
}

void Engine::slotStarted()
{
	emit engineMessage(tr("Engine started"));
	process->write("uci\n");
}

void Engine::slotStateChanged(QProcess::ProcessState newState)
{

}

void Engine::slotReadyRead()
{

}