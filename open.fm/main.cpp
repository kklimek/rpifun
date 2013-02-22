#include <QCoreApplication>

#include <QRpiGpio>

int main(int argc, char * argv[])
{
	QCoreApplication app(argc, argv);

	QRpiGpio gpio;

	return app.exec();
}
