#ifndef POWERMANAGER_H
#define POWERMANAGER_H

#include <QObject>
#include <QAction>

class UPower;


/*! \brief Simple UPower based backend interface.
\todo TODO/FIXME: HAL required too probably. Autodetection... whatever...
\todo TODO/FIXME: autodetection is mandatory! for example minimal suse does not run upower...
*/
class PowerManager : public QObject
{
    Q_OBJECT

public:
    PowerManager(QObject * parent);
    ~PowerManager();
    QList<QAction*> availableActions();
    
public slots:
    // upower
    void suspend();
    void hibernate();
    void reboot();
    void halt();
    // razor session
    void logout();

private:
    UPower * m_upower;

private slots:
    void infoMessage();
    void monitoring(const QString & msg);
};

#endif
