#include <stdio.h>
#include <errno.h>

#include <QMetaType>
#include <QMultiHash>
#include <QMutexLocker>
#include <QMutex>

#include <wiringPi.h>

#include "qrpigpio.h"

namespace QRpiGpioNS
{
	class Mapper;

	static QMutex sync;
	static Mapper * mapper = NULL;

	class PinInfo
	{
	public:
		PinInfo()
			: mode(QRpiGpio::PinMode_Unset)
		{
		}

		QRpiGpio::PinMode mode;
		QList<QRpiGpio *> listeners;
	};

	class Mapper
	{
	public:
		Mapper()
		{
			qRegisterMetaType<QRpiGpio::Pin>("QRpiGpio::Pin");
			qRegisterMetaType<QRpiGpio::PinValue>("QRpiGpio::PinValue");
			if (wiringPiSetup() != 0) {
				perror("wiringPiSetup");
			}
		}

		QHash<QRpiGpio::Pin, PinInfo> pinSetup;


		static void dispatch(QRpiGpio::Pin pin)
		{
			QMutexLocker lock(&QRpiGpioNS::sync);
			if (mapper->pinSetup[pin].mode == QRpiGpio::PinMode_Input) {
				QRpiGpio::PinValue val = digitalRead(pin) == HIGH ? QRpiGpio::PinValue_High : QRpiGpio::PinValue_Low;
				foreach(QRpiGpio * listener, mapper->pinSetup[pin].listeners) {
					QMetaObject::invokeMethod(listener, "interrupted", Qt::QueuedConnection, Q_ARG(QRpiGpio::Pin, pin), Q_ARG(QRpiGpio::PinValue, val));
					QMetaObject::invokeMethod(listener, val == QRpiGpio::PinValue_High ? "raised" : "fallen", Qt::QueuedConnection, Q_ARG(QRpiGpio::Pin, pin));
				}
			}
		}
	};
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
			QRpiGpioNS::Mapper *tmp = new QRpiGpioNS::Mapper();
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

QRpiGpio::~QRpiGpio()
{
	QMutexLocker lock(&QRpiGpioNS::sync);

	QHash<QRpiGpio::Pin, QRpiGpioNS::PinInfo>::iterator it;

	for(it = QRpiGpioNS::mapper->pinSetup.begin(); it != QRpiGpioNS::mapper->pinSetup.end(); it++) {
		it->listeners.removeOne(this);
	}
}

bool QRpiGpio::setOutput(QRpiGpio::Pin pin, QRpiGpio::PinValue initialValue)
{
	QMutexLocker lock(&QRpiGpioNS::sync);
	// For now we will support only one pin role per application run. No pin mode swapping
	if ( QRpiGpioNS::mapper->pinSetup[pin].mode == QRpiGpio::PinMode_Unset ) {
		QRpiGpioNS::mapper->pinSetup[pin].mode = QRpiGpio::PinMode_Output;
		pinMode(pin, OUTPUT);
		writePin(pin, initialValue);
		return true;
	}

	return false;
}

bool QRpiGpio::setInput(QRpiGpio::Pin pin)
{
	QMutexLocker lock(&QRpiGpioNS::sync);
	// For now we will support only one pin role per application run. No pin mode swapping
	if ( QRpiGpioNS::mapper->pinSetup[pin].mode == QRpiGpio::PinMode_Unset ) {
		QRpiGpioNS::mapper->pinSetup[pin].mode = QRpiGpio::PinMode_Input;
		pinMode(pin, INPUT);

		// setup callbacks

		switch(pin) {
		case QRpiGpio::Pin_GPIO0:
			wiringPiISR(pin, INT_EDGE_BOTH, []() { QRpiGpioNS::Mapper::dispatch(QRpiGpio::Pin_GPIO0); });
			break;

		case QRpiGpio::Pin_GPIO1:
			wiringPiISR(pin, INT_EDGE_BOTH, []() { QRpiGpioNS::Mapper::dispatch(QRpiGpio::Pin_GPIO1); });
			break;

		case QRpiGpio::Pin_GPIO2:
			wiringPiISR(pin, INT_EDGE_BOTH, []() { QRpiGpioNS::Mapper::dispatch(QRpiGpio::Pin_GPIO2); });
			break;

		case QRpiGpio::Pin_GPIO3:
			wiringPiISR(pin, INT_EDGE_BOTH, []() { QRpiGpioNS::Mapper::dispatch(QRpiGpio::Pin_GPIO3); });
			break;

		case QRpiGpio::Pin_GPIO4:
			wiringPiISR(pin, INT_EDGE_BOTH, []() { QRpiGpioNS::Mapper::dispatch(QRpiGpio::Pin_GPIO4); });
			break;

		case QRpiGpio::Pin_GPIO5:
			wiringPiISR(pin, INT_EDGE_BOTH, []() { QRpiGpioNS::Mapper::dispatch(QRpiGpio::Pin_GPIO5); });
			break;

		case QRpiGpio::Pin_GPIO6:
			wiringPiISR(pin, INT_EDGE_BOTH, []() { QRpiGpioNS::Mapper::dispatch(QRpiGpio::Pin_GPIO6); });
			break;

		case QRpiGpio::Pin_GPIO7:
			wiringPiISR(pin, INT_EDGE_BOTH, []() { QRpiGpioNS::Mapper::dispatch(QRpiGpio::Pin_GPIO7); });
			break;
		}
	}

	if ( QRpiGpioNS::mapper->pinSetup[pin].mode == QRpiGpio::PinMode_Input ) {
		if ( ! QRpiGpioNS::mapper->pinSetup[pin].listeners.contains(this) ) {
			QRpiGpioNS::mapper->pinSetup[pin].listeners << this;
			return true;
		}
	}

	// return false even when pin is setup but called again from same instance
	return false;
}

bool QRpiGpio::writePin(QRpiGpio::Pin pin, QRpiGpio::PinValue val)
{
	if ( QRpiGpioNS::mapper->pinSetup[pin].mode == QRpiGpio::PinMode_Output ) {
		digitalWrite(pin, val);
		return true;
	}

	return false;
}

void QRpiGpio::delayMs(quint32 miliseconds)
{
	delay(miliseconds);
}

void QRpiGpio::delayUs(quint32 microseconds)
{
	delayMicroseconds(microseconds);
}

