#include <QTimer>
#include <QDebug>

#include "qrpip1header.h"

#include "qrpitactswitch.h"

namespace QRpi {

class TactSwitchPrivate: public QObject
{
	Q_OBJECT
public:
	TactSwitchPrivate(P1Header::Pin p, TactSwitch * parent)
		: QObject(parent)
		, q_ptr(parent)
		, header(new P1Header(this))
	{
		debounce[P1Header::PinValue_High] = NULL;
		debounce[P1Header::PinValue_Low] = NULL;

		QHash<P1Header::PinValue, QTimer *>::iterator it;

		for(it = debounce.begin(); it != debounce.end(); it++) {
			QTimer * timer = new QTimer(this);
			timer->setSingleShot(true);
			timer->setInterval(20);
			*it = timer;
		}

		connect(header, SIGNAL(interrupted(QRpi::P1Header::Pin, QRpi::P1Header::PinValue)), this, SLOT(interrupted(QRpi::P1Header::Pin, QRpi::P1Header::PinValue)));

		header->setInput(p);
	}

public:
	TactSwitch * const q_ptr;
	P1Header * header;

	QHash<P1Header::PinValue, QTimer *> debounce;

public slots:
	void interrupted(QRpi::P1Header::Pin pin, QRpi::P1Header::PinValue val)
	{
		// TODO: clicked, doubleClicked, longClicked
		// TODO: event order - map high/low to pressed/released
		if ( ! debounce[val]->isActive() ) {
			debounce[val]->start();

			switch(val) {
				case P1Header::PinValue_High:
					emit pressed();
					break;

				case P1Header::PinValue_Low:
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

TactSwitch::TactSwitch(P1Header::Pin p, QObject * parent)
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

#include "qrpitactswitch.moc"

} // namespace QRpi
