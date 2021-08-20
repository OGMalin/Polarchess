#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QCoreApplication::setOrganizationName("PolarChess");
	QCoreApplication::setApplicationName("Gui");
	QCoreApplication::setApplicationVersion("0.1");
	app.setApplicationDisplayName("Polarchess");

	// For timer function to behave
	BOOL b = FALSE;
	if (!SetUserObjectInformation(GetCurrentProcess(), UOI_TIMERPROC_EXCEPTION_SUPPRESSION, &b, sizeof(BOOL)))
		qDebug("Failed to set TimerProc exception supression");
	MainWindow w;
	w.show();
	w.firstTime();
	return app.exec();
}
