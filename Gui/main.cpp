#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QCoreApplication::setOrganizationName("PolarChess");
	QCoreApplication::setApplicationName("Gui");
	QCoreApplication::setApplicationVersion("0.1");
	app.setApplicationDisplayName("Polarchess");

	MainWindow w;
	w.show();
	w.firstTime();
	return app.exec();
}
