#include <stdio.h>
#include <errno.h>

#include <QMutexLocker>
#include <QMutex>

#include <wiringPi.h>

#include "qrpigpio.h"

namespace QRpiGpioNS
{
	class QRpiGpioMapper
	{
	public:
		QRpiGpioMapper()
		{
			if (wiringPiSetup() != 0) {
				perror("wiringPiSetup:");
			}
		}
	};

	static QRpiGpioMapper * mapper = NULL;
	static QMutex sync;
}


class QRpiGpioPrivate
{
public:
	QRpiGpioPrivate(QRpiGpio * parent)
		: q_ptr(parent)
	{
		if ( ! QRpiGpioNS::mapper ) {
			QMutexLocker lock(&QRpiGpioNS::sync);
			if ( ! QRpiGpioNS::mapper ) {
				// Compiler specific issue. Standard does not say if constructor is called after or before pointer is assigned
				QRpiGpioNS::QRpiGpioMapper *tmp = new QRpiGpioNS::QRpiGpioMapper();
				QRpiGpioNS::mapper = tmp;
			}
		}
	}

	Q_DECLARE_PUBLIC(QRpiGpio);
	QRpiGpio * const q_ptr;
};

QRpiGpio::QRpiGpio(QObject * parent)
	: QObject(parent)
	, d_ptr(new QRpiGpioPrivate(this))
{
}
