#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QCoreApplication::setOrganizationName("PolarChess");
	QCoreApplication::setApplicationName("Test");
	QCoreApplication::setApplicationVersion("0.1");
	app.setApplicationDisplayName("PolarTest");

	MainWindow w;
	w.show();
	return app.exec();
}
