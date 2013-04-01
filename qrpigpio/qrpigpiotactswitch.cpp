#include <QTimer>
#include <QDateTime>
#include <QDebug>

#include "qrpigpiop1header.h"

#include "qrpigpiotactswitch.h"

namespace QRpiGpio {

class TactSwitchPrivate: public QObject
{
	Q_OBJECT
public:
	TactSwitchPrivate(Pin p, PinValue pressed, TactSwitch * parent)
		: QObject(parent)
		, q_ptr(parent)
		, header(new P1Header(this))
		, debounceInterval(50)
		, pressedValue(pressed)
	{
		switch(pressedValue) {
		case PinValue_High:
			currentValue = PinValue_Low;
			break;

		case PinValue_Low:
			currentValue = PinValue_High;
			break;
		}

		connect(header, SIGNAL(interrupted(QRpiGpio::Pin, QRpiGpio::PinValue)), this, SLOT(interrupted(QRpiGpio::Pin, QRpiGpio::PinValue)));

		header->setInput(p);

		connect(this, SIGNAL(pressed()), q_ptr, SIGNAL(pressed()));
		connect(this, SIGNAL(released()), q_ptr, SIGNAL(released()));
		connect(this, SIGNAL(clicked()), q_ptr, SIGNAL(clicked()));
		connect(this, SIGNAL(doubleClicked()), q_ptr, SIGNAL(doubleClicked()));
		connect(this, SIGNAL(longPressed()), q_ptr, SIGNAL(longPressed()));

		longPressedTimer.setSingleShot(true);
		longPressedTimer.setInterval(400);
		clickedTimer.setSingleShot(true);
		clickedTimer.setInterval(400);

		connect(&longPressedTimer, SIGNAL(timeout()), this, SLOT(emitLongPressed()));
		connect(&clickedTimer, SIGNAL(timeout()), this, SIGNAL(clicked()));

		pDebounce = rDebounce = QDateTime::currentDateTimeUtc().addMSecs(-1 * debounceInterval);
	}

public:
	TactSwitch * const q_ptr;
	P1Header * header;
	quint16 debounceInterval;
	QTimer longPressedTimer;
	QTimer clickedTimer;

	QDateTime pDebounce;
	QDateTime rDebounce;

	PinValue pressedValue;
	PinValue currentValue;

public slots:
	void interrupted(QRpiGpio::Pin pin, QRpiGpio::PinValue val)
	{
		Q_UNUSED(pin); // we get only one pin info from header
		QDateTime curr = QDateTime::currentDateTimeUtc();

		if ( val != currentValue ) {
			if ( val == pressedValue ) {
				if ( pDebounce.msecsTo(curr) < debounceInterval ) {
					qDebug() << Q_FUNC_INFO << "pressed debounce! ignoring";
					return;
				}

				pDebounce = curr;
				longPressedTimer.start();
				emit pressed();
			} else {
				if ( rDebounce.msecsTo(curr) < debounceInterval ) {
					qDebug() << Q_FUNC_INFO << "pressed debounce! ignoring";
					return;
				}

				rDebounce = curr;
				longPressedTimer.stop();
				emit released();
				if(clickedTimer.isActive()) {
					clickedTimer.stop();
					emit doubleClicked();
				} else {
					clickedTimer.start();
				}
			}

			currentValue = val;
		}
	}

	void emitLongPressed()
	{
		// switch current value so release will be ignored
		switch(pressedValue) {
		case PinValue_High:
			currentValue = PinValue_Low;
			break;

		case PinValue_Low:
			currentValue = PinValue_High;
			break;
		}

		emit longPressed();
	}

signals:
	void pressed();
	void released();

	void clicked();
	void doubleClicked();
	void longPressed();
};

TactSwitch::TactSwitch(Pin p, QObject * parent)
	: QObject(parent)
	, d_ptr(new TactSwitchPrivate(p, PinValue_High, this))
{
}

TactSwitch::TactSwitch(Pin p, PinValue pressedValue, QObject * parent)
	: QObject(parent)
	, d_ptr(new TactSwitchPrivate(p, pressedValue, this))
{
}

void TactSwitch::setDebounceInterval(quint16 miliseconds)
{
	Q_D(TactSwitch);
	d->debounceInterval = miliseconds;
	d->rDebounce = QDateTime::currentDateTimeUtc().addMSecs(-1 * d->debounceInterval);
	d->pDebounce = d->rDebounce;
}

void TactSwitch::setDoubleClickInterval(quint16 miliseconds)
{
	Q_D(TactSwitch);
	d->clickedTimer.setInterval(miliseconds);
}

void TactSwitch::setLongPressedInterval(quint16 miliseconds)
{
	Q_D(TactSwitch);
	d->longPressedTimer.setInterval(miliseconds);
}

#include "qrpigpiotactswitch.moc"

} // namespace QRpiGpio
