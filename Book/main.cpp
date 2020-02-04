#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QCoreApplication::setOrganizationName("PolarChess");
	QCoreApplication::setApplicationName("Book");
	QCoreApplication::setApplicationVersion("0.1");
//	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	app.setApplicationDisplayName("PolarBook");

	MainWindow w;
	w.show();
	return app.exec();
}
