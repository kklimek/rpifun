#ifndef __QRPIGPIOP1HEADER_H_
#define __QRPIGPIOP1HEADER_H_

#include <QObject>

#include <QRpiGpioGlobal>

namespace QRpiGpio {

class P1HeaderPrivate;

class P1Header: public QObject
{
Q_OBJECT


public:
	P1Header(QObject * parent = NULL);
	~P1Header();

	bool setOutput(Pin, PinValue initialValue = PinValue_High);
	bool setInput(Pin);
	bool writePin(Pin, PinValue);
	PinValue readPin(Pin);

	void delayMs(quint32 miliseconds);
	void delayUs(quint32 microseconds);

signals:
	void interrupted(QRpiGpio::Pin, QRpiGpio::PinValue);
	void raised(QRpiGpio::Pin);
	void fallen(QRpiGpio::Pin);

private:
	Q_DECLARE_PRIVATE(P1Header);
	P1HeaderPrivate * const d_ptr;
};

} // namespace QRpiGpio
#endif // __QRPIGPIOP1HEADER_H_

