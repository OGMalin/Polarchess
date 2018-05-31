#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QCoreApplication::setOrganizationName("PolarChess");
	QCoreApplication::setApplicationName("PolarChess");
	QCoreApplication::setApplicationVersion("1.0 beta 1");
	app.setApplicationDisplayName("Polarchess");
	char* psz = new char[10];
	MainWindow w;
	w.show();
	return app.exec();
}
