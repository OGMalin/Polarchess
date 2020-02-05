#include "MainWindow.h"
#include <QApplication>
#include <QGuiApplication>
//#include <QtGlobal>

int main(int argc, char *argv[])
{
	QCoreApplication::setOrganizationName("PolarChess");
	QCoreApplication::setApplicationName("Book");
	QCoreApplication::setApplicationVersion("0.1");
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
//#if QT_VERSION >= 0x051400
//	QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
//#endif
	QApplication app(argc, argv);
	app.setApplicationDisplayName("PolarBook");

	MainWindow w;
	w.show();
	return app.exec();
}
