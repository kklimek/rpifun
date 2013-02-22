#include <QCoreApplication>
#include <QByteArray>
#include <QString>

#include <QRpiGpio>

#define LCD_RS QRpiGpio::Pin_CE1
#define LCD_E  QRpiGpio::Pin_CE0
#define LCD_D4 QRpiGpio::Pin_GPIO6
#define LCD_D5 QRpiGpio::Pin_GPIO5
#define LCD_D6 QRpiGpio::Pin_GPIO4
#define LCD_D7 QRpiGpio::Pin_GPIO1

#define LCD_MODE_CMD QRpiGpio::PinValue_High
#define LCD_MODE_CHR QRpiGpio::PinValue_Low

#define LCD_LINE_1 0x80
#define LCD_LINE_2 0xC0


void lcd_pulse(QRpiGpio * gpio)
{
	gpio->delayUs(50);
}

void lcd_delay(QRpiGpio * gpio)
{
	gpio->delayUs(50);
}

void lcd_reset_data(QRpiGpio * gpio)
{
	gpio->writePin(LCD_D4, QRpiGpio::PinValue_High);
	gpio->writePin(LCD_D5, QRpiGpio::PinValue_High);
	gpio->writePin(LCD_D6, QRpiGpio::PinValue_High);
	gpio->writePin(LCD_D7, QRpiGpio::PinValue_High);
}

void lcd_toggle_enable(QRpiGpio * gpio)
{
	lcd_delay(gpio);
	gpio->writePin(LCD_E, QRpiGpio::PinValue_Low);
	lcd_pulse(gpio);
	gpio->writePin(LCD_E, QRpiGpio::PinValue_High);
	lcd_delay(gpio);
}

void lcd_byte(QRpiGpio * gpio, int bits,  QRpiGpio::PinValue mode)
{
	gpio->writePin(LCD_RS, mode);

	lcd_reset_data(gpio);

	if ((bits & 0x10) == 0x10)
		gpio->writePin(LCD_D4, QRpiGpio::PinValue_Low);


	if ((bits & 0x20) == 0x20)
		gpio->writePin(LCD_D5, QRpiGpio::PinValue_Low);

	if ((bits & 0x40) == 0x40)
		gpio->writePin(LCD_D6, QRpiGpio::PinValue_Low);

	if ((bits & 0x80) == 0x80)
		gpio->writePin(LCD_D7, QRpiGpio::PinValue_Low);


	lcd_toggle_enable(gpio);

	lcd_reset_data(gpio);

	if ((bits & 0x01) == 0x01)
		gpio->writePin(LCD_D4, QRpiGpio::PinValue_Low);

	if ((bits & 0x02) == 0x02)
		gpio->writePin(LCD_D5, QRpiGpio::PinValue_Low);

	if ((bits & 0x04) == 0x04)
		gpio->writePin(LCD_D6, QRpiGpio::PinValue_Low);

	if ((bits & 0x08) == 0x08)
		gpio->writePin(LCD_D7, QRpiGpio::PinValue_Low);

	lcd_toggle_enable(gpio);
}

void lcd_init(QRpiGpio * gpio)
{
	lcd_byte(gpio, 0x33, LCD_MODE_CMD);
	lcd_byte(gpio, 0x32, LCD_MODE_CMD);
	lcd_byte(gpio, 0x28, LCD_MODE_CMD);
	lcd_byte(gpio, 0x0c, LCD_MODE_CMD);
	lcd_byte(gpio, 0x06, LCD_MODE_CMD);
	lcd_byte(gpio, 0x01, LCD_MODE_CMD);
}

void lcd_write_string(QRpiGpio *gpio, QString str, bool secondLine = false)
{
	lcd_byte(gpio, secondLine ? LCD_LINE_2 : LCD_LINE_1, LCD_MODE_CMD);

	str = str.left(16);

	for(int i = 0; i < str.size(); ++i) {
		qDebug("%d", str.at(i).unicode());
		lcd_byte(gpio, str.at(i).unicode(), LCD_MODE_CHR);
	}
}

int main(int argc, char * argv[])
{
	QCoreApplication app(argc, argv);

	QRpiGpio gpio;
	gpio.setPinMode(LCD_E, QRpiGpio::PinMode_Output);
	gpio.setPinMode(LCD_RS, QRpiGpio::PinMode_Output);
	gpio.setPinMode(LCD_D4, QRpiGpio::PinMode_Output);
	gpio.setPinMode(LCD_D5, QRpiGpio::PinMode_Output);
	gpio.setPinMode(LCD_D6, QRpiGpio::PinMode_Output);
	gpio.setPinMode(LCD_D7, QRpiGpio::PinMode_Output);

	lcd_init(&gpio);

	lcd_write_string(&gpio, "123");
	lcd_write_string(&gpio, "Testing", true);

	return 0;
}
