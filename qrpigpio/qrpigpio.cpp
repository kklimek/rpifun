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
		QMutexLocker lock(&QRpiGpioNS::sync);
		if ( ! QRpiGpioNS::mapper ) {
			// Compiler specific issue. Standard does not say if constructor is called after or before pointer is assigned
			QRpiGpioNS::QRpiGpioMapper *tmp = new QRpiGpioNS::QRpiGpioMapper();
			QRpiGpioNS::mapper = tmp;
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

void QRpiGpio::setPinMode(QRpiGpio::Pin pin, QRpiGpio::PinMode mode, QRpiGpio::PinValue initialValue)
{
	int val = -1;

	switch(mode) {
		case QRpiGpio::PinMode_Input:
			val = INPUT;
			break;

		case QRpiGpio::PinMode_Output:
			val = OUTPUT;
			break;
	}

	if (val != -1) {
		pinMode(pin, val);
		writePin(pin, initialValue);
	}
}

void QRpiGpio::writePin(QRpiGpio::Pin pin, QRpiGpio::PinValue val)
{
	digitalWrite(pin, val);
}

void QRpiGpio::delayMs(quint32 miliseconds)
{
	delay(miliseconds);
}

void QRpiGpio::delayUs(quint32 microseconds)
{
	delayMicroseconds(microseconds);
}

