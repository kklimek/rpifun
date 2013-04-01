#ifndef __QRPITACTSWITCH_H_
#define __QRPITACTSWITCH_H_

#include <QObject>

#include <QRpiGpioGlobal>

// TODO: docs!
namespace QRpiGpio {

class TactSwitchPrivate;

class TactSwitch: public QObject
{
	Q_OBJECT
public:
	TactSwitch(Pin p, QObject * parent = NULL);
	TactSwitch(Pin p, PinValue pressedValue, QObject * parent = NULL);
	void setDebounceInterval(quint16 miliseconds);
	void setDoubleClickInterval(quint16 miliseconds);
	void setLongPressedInterval(quint16 miliseconds);

signals:
	void pressed();
	void released();

	void clicked();
	void doubleClicked();
	void longPressed();

private:
	Q_DECLARE_PRIVATE(TactSwitch);
	TactSwitchPrivate * const d_ptr;
};

} // namespace QRpiGpio

#endif // __QRPITACTSWITCH_H_

