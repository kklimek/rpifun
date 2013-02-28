#include <stdio.h>
#include <errno.h>

#include <QMetaType>
#include <QMultiHash>
#include <QMutexLocker>
#include <QMutex>

#include <wiringPi.h>

#include "qrpigpiop1header.h"

namespace QRpiGpio {

namespace P1HeaderNS {
	class Mapper;

	static QMutex sync;
	static Mapper * mapper = NULL;

	class PinInfo
	{
	public:
		PinInfo()
			: mode(PinMode_Unset)
		{
		}

		PinMode mode;
		QList<P1Header *> listeners;
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

		QHash<Pin, PinInfo> pinSetup;


		static void dispatch(Pin pin)
		{
			QMutexLocker lock(&P1HeaderNS::sync);
			if (mapper->pinSetup[pin].mode == PinMode_Input) {
				PinValue val = digitalRead(pin) == HIGH ? PinValue_High : PinValue_Low;
				foreach(P1Header * listener, mapper->pinSetup[pin].listeners) {
					QMetaObject::invokeMethod(listener, "interrupted", Qt::QueuedConnection, Q_ARG(QRpiGpio::Pin, pin), Q_ARG(QRpiGpio::PinValue, val));
					QMetaObject::invokeMethod(listener, val == PinValue_High ? "raised" : "fallen", Qt::QueuedConnection, Q_ARG(QRpiGpio::Pin, pin));
				}
			}
		}
	};
} // namespace P1HeaderNS


class P1HeaderPrivate
{
public:
	P1HeaderPrivate(P1Header * parent)
		: q_ptr(parent)
	{
		QMutexLocker lock(&P1HeaderNS::sync);
		if ( ! P1HeaderNS::mapper ) {
			// Compiler specific issue. Standard does not say if constructor is called after or before pointer is assigned
			P1HeaderNS::Mapper *tmp = new P1HeaderNS::Mapper();
			P1HeaderNS::mapper = tmp;
		}
	}

	Q_DECLARE_PUBLIC(P1Header);
	P1Header * const q_ptr;
};

P1Header::P1Header(QObject * parent)
	: QObject(parent)
	, d_ptr(new P1HeaderPrivate(this))
{
}

P1Header::~P1Header()
{
	QMutexLocker lock(&P1HeaderNS::sync);

	QHash<Pin, P1HeaderNS::PinInfo>::iterator it;

	for(it = P1HeaderNS::mapper->pinSetup.begin(); it != P1HeaderNS::mapper->pinSetup.end(); it++) {
		it->listeners.removeOne(this);
	}
}

bool P1Header::setOutput(Pin pin, PinValue initialValue)
{
	QMutexLocker lock(&P1HeaderNS::sync);
	// For now we will support only one pin role per application run. No pin mode swapping
	if ( P1HeaderNS::mapper->pinSetup[pin].mode == PinMode_Unset ) {
		P1HeaderNS::mapper->pinSetup[pin].mode = PinMode_Output;
		pinMode(pin, OUTPUT);
		writePin(pin, initialValue);
		return true;
	}

	return false;
}

bool P1Header::setInput(Pin pin)
{
	QMutexLocker lock(&P1HeaderNS::sync);
	// For now we will support only one pin role per application run. No pin mode swapping
	if ( P1HeaderNS::mapper->pinSetup[pin].mode == PinMode_Unset ) {
		P1HeaderNS::mapper->pinSetup[pin].mode = PinMode_Input;
		pinMode(pin, INPUT);

		// setup callbacks

		switch(pin) {
		case Pin_GPIO0:
			wiringPiISR(pin, INT_EDGE_BOTH, []() { P1HeaderNS::Mapper::dispatch(Pin_GPIO0); });
			break;

		case Pin_GPIO1:
			wiringPiISR(pin, INT_EDGE_BOTH, []() { P1HeaderNS::Mapper::dispatch(Pin_GPIO1); });
			break;

		case Pin_GPIO2:
			wiringPiISR(pin, INT_EDGE_BOTH, []() { P1HeaderNS::Mapper::dispatch(Pin_GPIO2); });
			break;

		case Pin_GPIO3:
			wiringPiISR(pin, INT_EDGE_BOTH, []() { P1HeaderNS::Mapper::dispatch(Pin_GPIO3); });
			break;

		case Pin_GPIO4:
			wiringPiISR(pin, INT_EDGE_BOTH, []() { P1HeaderNS::Mapper::dispatch(Pin_GPIO4); });
			break;

		case Pin_GPIO5:
			wiringPiISR(pin, INT_EDGE_BOTH, []() { P1HeaderNS::Mapper::dispatch(Pin_GPIO5); });
			break;

		case Pin_GPIO6:
			wiringPiISR(pin, INT_EDGE_BOTH, []() { P1HeaderNS::Mapper::dispatch(Pin_GPIO6); });
			break;

		case Pin_GPIO7:
			wiringPiISR(pin, INT_EDGE_BOTH, []() { P1HeaderNS::Mapper::dispatch(Pin_GPIO7); });
			break;
		}
	}

	if ( P1HeaderNS::mapper->pinSetup[pin].mode == PinMode_Input ) {
		if ( ! P1HeaderNS::mapper->pinSetup[pin].listeners.contains(this) ) {
			P1HeaderNS::mapper->pinSetup[pin].listeners << this;
			return true;
		}
	}

	// return false even when pin is setup but called again from same instance
	return false;
}

bool P1Header::writePin(Pin pin, PinValue val)
{
	if ( P1HeaderNS::mapper->pinSetup[pin].mode == PinMode_Output ) {
		digitalWrite(pin, val);
		return true;
	}

	return false;
}

void P1Header::delayMs(quint32 miliseconds)
{
	delay(miliseconds);
}

void P1Header::delayUs(quint32 microseconds)
{
	delayMicroseconds(microseconds);
}

} // namespace QRpiGpio
