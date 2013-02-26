#ifndef __QRPITACTSWITCH_H_
#define __QRPITACTSWITCH_H_

#include <QObject>

#include <QRpiP1Header>

namespace QRpi {

class TactSwitchPrivate;

class TactSwitch: public QObject
{
	Q_OBJECT
public:
	// TODO: HIGH/LOW -> PRESSED/RELEASED mapping
	TactSwitch(P1Header::Pin p, QObject * parent = NULL);
	void setDebounceInterval(int miliseconds);
	// > DebounceInterval
	void setDoubleClickInterval(int miliseconds);
	void setLongClickedInterval(int miliseconds);

signals:
	void pressed();
	void released();

	void clicked();
	void doubleClicked();
	void longClicked();

private:
	Q_DECLARE_PRIVATE(TactSwitch);
	TactSwitchPrivate * const d_ptr;
};

} // namespace QRpi

#endif // __QRPITACTSWITCH_H_

