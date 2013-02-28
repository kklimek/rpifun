#include <QTimer>
#include <QDebug>

#include "qrpigpiop1header.h"

#include "qrpigpiotactswitch.h"

namespace QRpiGpio {

class TactSwitchPrivate: public QObject
{
	Q_OBJECT
public:
	TactSwitchPrivate(Pin p, TactSwitch * parent)
		: QObject(parent)
		, q_ptr(parent)
		, header(new P1Header(this))
	{
		debounce[PinValue_High] = NULL;
		debounce[PinValue_Low] = NULL;

		QHash<PinValue, QTimer *>::iterator it;

		for(it = debounce.begin(); it != debounce.end(); it++) {
			QTimer * timer = new QTimer(this);
			timer->setSingleShot(true);
			timer->setInterval(20);
			*it = timer;
		}

		connect(header, SIGNAL(interrupted(QRpiGpio::Pin, QRpiGpio::PinValue)), this, SLOT(interrupted(QRpiGpio::Pin, QRpiGpio::PinValue)));

		header->setInput(p);
	}

public:
	TactSwitch * const q_ptr;
	P1Header * header;

	QHash<PinValue, QTimer *> debounce;

public slots:
	void interrupted(QRpiGpio::Pin pin, QRpiGpio::PinValue val)
	{
		// TODO: clicked, doubleClicked, longClicked
		// TODO: event order - map high/low to pressed/released
		if ( ! debounce[val]->isActive() ) {
			debounce[val]->start();

			switch(val) {
				case PinValue_High:
					emit pressed();
					break;

				case PinValue_Low:
					emit released();
					break;
			}
		} else {
			qDebug() << Q_FUNC_INFO << q_ptr << "bounce, ignoring";
		}
	}

signals:
	void pressed();
	void released();

	void clicked();
	void doubleClicked();
	void longClicked();
};

TactSwitch::TactSwitch(Pin p, QObject * parent)
	: QObject(parent)
	, d_ptr(new TactSwitchPrivate(p, this))
{
	connect(d_ptr, SIGNAL(pressed()), this, SIGNAL(pressed()));
	connect(d_ptr, SIGNAL(released()), this, SIGNAL(released()));
	connect(d_ptr, SIGNAL(clicked()), this, SIGNAL(clicked()));
	connect(d_ptr, SIGNAL(doubleClicked()), this, SIGNAL(doubleClicked()));
	connect(d_ptr, SIGNAL(longClicked()), this, SIGNAL(longClicked()));
}

void TactSwitch::setDebounceInterval(int miliseconds)
{
	foreach(QTimer * timer, d_ptr->debounce) {
		timer->setInterval(miliseconds);
	}
}

#include "qrpigpiotactswitch.moc"

} // namespace QRpiGpio
