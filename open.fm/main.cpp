#include <QCoreApplication>
#include <QByteArray>
#include <QString>
#include <QDateTime>
#include <QDebug>

#include <QRpiGpio>

#define LCD_RS QRpi::P1Header::Pin_CE1
#define LCD_E  QRpi::P1Header::Pin_CE0
#define LCD_D4 QRpi::P1Header::Pin_GPIO6
#define LCD_D5 QRpi::P1Header::Pin_GPIO5
#define LCD_D6 QRpi::P1Header::Pin_GPIO4
#define LCD_D7 QRpi::P1Header::Pin_GPIO1

#define LCD_MODE_CMD QRpi::P1Header::PinValue_High
#define LCD_MODE_CHR QRpi::P1Header::PinValue_Low

#define LCD_LINE_1 0x80
#define LCD_LINE_2 0xC0


class DebugOutput: public QObject
{
	Q_OBJECT
public slots:
	void interrupted(QRpi::P1Header::Pin pin, QRpi::P1Header::PinValue val) {
		qDebug() << Q_FUNC_INFO << pin << val;
	}

	void fallen(QRpi::P1Header::Pin pin) {
		qDebug() << Q_FUNC_INFO << pin;
	}

	void raised(QRpi::P1Header::Pin pin) {
		qDebug() << Q_FUNC_INFO << pin;
	}

	void pressed() {
		qDebug() << Q_FUNC_INFO << sender();
	}

	void released() {
		qDebug() << Q_FUNC_INFO << sender();
	}
};

void lcd_pulse(QRpi::P1Header * gpio)
{
	gpio->delayUs(50);
}

void lcd_delay(QRpi::P1Header * gpio)
{
	gpio->delayUs(50);
}

void lcd_reset_data(QRpi::P1Header * gpio)
{
	gpio->writePin(LCD_D4, QRpi::P1Header::PinValue_High);
	gpio->writePin(LCD_D5, QRpi::P1Header::PinValue_High);
	gpio->writePin(LCD_D6, QRpi::P1Header::PinValue_High);
	gpio->writePin(LCD_D7, QRpi::P1Header::PinValue_High);
}

void lcd_toggle_enable(QRpi::P1Header * gpio)
{
	lcd_delay(gpio);
	gpio->writePin(LCD_E, QRpi::P1Header::PinValue_Low);
	lcd_pulse(gpio);
	gpio->writePin(LCD_E, QRpi::P1Header::PinValue_High);
	lcd_delay(gpio);
}

void lcd_byte(QRpi::P1Header * gpio, int bits,  QRpi::P1Header::PinValue mode)
{
	gpio->writePin(LCD_RS, mode);

	lcd_reset_data(gpio);

	if ((bits & 0x10) == 0x10)
		gpio->writePin(LCD_D4, QRpi::P1Header::PinValue_Low);


	if ((bits & 0x20) == 0x20)
		gpio->writePin(LCD_D5, QRpi::P1Header::PinValue_Low);

	if ((bits & 0x40) == 0x40)
		gpio->writePin(LCD_D6, QRpi::P1Header::PinValue_Low);

	if ((bits & 0x80) == 0x80)
		gpio->writePin(LCD_D7, QRpi::P1Header::PinValue_Low);


	lcd_toggle_enable(gpio);

	lcd_reset_data(gpio);

	if ((bits & 0x01) == 0x01)
		gpio->writePin(LCD_D4, QRpi::P1Header::PinValue_Low);

	if ((bits & 0x02) == 0x02)
		gpio->writePin(LCD_D5, QRpi::P1Header::PinValue_Low);

	if ((bits & 0x04) == 0x04)
		gpio->writePin(LCD_D6, QRpi::P1Header::PinValue_Low);

	if ((bits & 0x08) == 0x08)
		gpio->writePin(LCD_D7, QRpi::P1Header::PinValue_Low);

	lcd_toggle_enable(gpio);
}

void lcd_init(QRpi::P1Header * gpio)
{
	lcd_byte(gpio, 0x33, LCD_MODE_CMD);
	lcd_byte(gpio, 0x32, LCD_MODE_CMD);
	lcd_byte(gpio, 0x28, LCD_MODE_CMD);
	lcd_byte(gpio, 0x0c, LCD_MODE_CMD);
	lcd_byte(gpio, 0x06, LCD_MODE_CMD);
	lcd_byte(gpio, 0x01, LCD_MODE_CMD);
}

void lcd_write_string(QRpi::P1Header *gpio, QString str, bool secondLine = false)
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

	DebugOutput out;
	QRpi::P1Header gpio;

	gpio.setOutput(LCD_E, QRpi::P1Header::PinValue_High);
	gpio.setOutput(LCD_RS, QRpi::P1Header::PinValue_High);
	gpio.setOutput(LCD_D4, QRpi::P1Header::PinValue_High);
	gpio.setOutput(LCD_D5, QRpi::P1Header::PinValue_High);
	gpio.setOutput(LCD_D6, QRpi::P1Header::PinValue_High);
	gpio.setOutput(LCD_D7, QRpi::P1Header::PinValue_High);

	lcd_init(&gpio);

	lcd_write_string(&gpio, "123");
	lcd_write_string(&gpio, "Testing", true);


	QRpi::TactSwitch tact(QRpi::P1Header::Pin_GPIO7);
	tact.setDebounceInterval(200); // I bought reeaaaaaly crappy tact switches

	QObject::connect(&tact, SIGNAL(pressed()), &out, SLOT(pressed()));
	QObject::connect(&tact, SIGNAL(released()), &out, SLOT(released()));

	return app.exec();
}

#include "main.moc"
