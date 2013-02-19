#include <QCoreApplication>

#include <QGpio>

int main(int argc, char * argv[])
{
	QCoreApplication app(argc, argv);

	QGpio gpio;

	return app.exec();
}
