#ifndef __QRPIGPIO_H_
#define __QRPIGPIO_H_

#include <QObject>

class QRpiGpioPrivate;

class QRpiGpio: public QObject
{
Q_OBJECT

public:
enum PinValue
{
	PinValue_High = 0,
	PinValue_Low  = 1
};

public:
/**
 * Pin mapping reflecting wiringPi pin numbers
 * https://projects.drogon.net/raspberry-pi/wiringpi/pins/
 */
enum Pin {
	Pin_GPIO0 = 0,
	Pin_GPIO1 = 1,
	Pin_GPIO2 = 2,
	Pin_GPIO3 = 3,
	Pin_GPIO4 = 4,
	Pin_GPIO5 = 5,
	Pin_GPIO6 = 6,
	Pin_GPIO7 = 7,
	Pin_SDA0  = 8,
	Pin_SCL0  = 9,
	Pin_CE0   = 10,
	Pin_CE1   = 11,
	Pin_MOSI  = 12,
	Pin_MISO  = 13,
	Pin_SCLK  = 14,
	Pin_TxD   = 15,
	Pin_RxD   = 16
};

public:
enum PinMode {
	PinMode_Input,
	PinMode_Output
};

public:
	QRpiGpio(QObject * parent = NULL);

	void setPinMode(Pin, PinMode, PinValue initWithValue = PinValue_High);
	void writePin(Pin, PinValue);

	void delayMs(quint32 miliseconds);
	void delayUs(quint32 microseconds);

private:
	Q_DECLARE_PRIVATE(QRpiGpio);
	QRpiGpioPrivate * const d_ptr;
};

#endif // __QRPIGPIO_H_

