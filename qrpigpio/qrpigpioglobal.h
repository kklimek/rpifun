#ifndef __QRPIGPIOGLOBAL_H_
#define __QRPIGPIOGLOBAL_H_

namespace QRpiGpio {

enum PinValue
{
	PinValue_High = 0,
	PinValue_Low  = 1
};

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

enum PinMode {
	PinMode_Unset,
	PinMode_Input,
	PinMode_Output
};
} //

#endif // __QRPIGPIOGLOBAL_H_
