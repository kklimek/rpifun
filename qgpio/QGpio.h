#ifndef __Q_GPIO_H_
#define __Q_GPIO_H_

#include <QObject>

class QGpioPrivate;

class QGpio: public QObject
{
Q_OBJECT

public:
	static QGpio & instance();

private:
	QGpio();

private:
	Q_DECLARE_PRIVATE(QGpio);
	QGpioPrivate * const d_ptr;
};

#endif // __Q_GPIO_H_

