#include <stdio.h>
#include <errno.h>
#include <QMutexLocker>
#include <QMutex>

#include <wiringPi.h>

#include "qgpio.h"

namespace QGpioNS
{
	class QGpioMapper
	{
	public:
		QGpioMapper()
		{
			if (wiringPiSetup() != 0) {
				perror("wiringPiSetup:");
			}
		}
	};

	static QGpioMapper * mapper = NULL;
	static QMutex sync;
}


class QGpioPrivate
{
public:
	QGpioPrivate(QGpio * parent)
		: q_ptr(parent)
	{
		if ( ! QGpioNS::mapper ) {
			QMutexLocker lock(&QGpioNS::sync);
			if ( ! QGpioNS::mapper ) {
				// Compiler specific issue. Standard does not say if constructor is called after or before pointer is assigned
				QGpioNS::QGpioMapper *tmp = new QGpioNS::QGpioMapper();
				QGpioNS::mapper = tmp;
			}
		}
	}

	Q_DECLARE_PUBLIC(QGpio);
	QGpio * const q_ptr;
};

QGpio::QGpio(QObject * parent)
	: QObject(parent)
	, d_ptr(new QGpioPrivate(this))
{
}
