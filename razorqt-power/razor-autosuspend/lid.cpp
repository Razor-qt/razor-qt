#include "lid.h"
#include <QDBusConnection>
#include <QDBusReply>
#include <QDebug>

Lid::Lid()
{
    uPower = new QDBusInterface("org.freedesktop.UPower", "/org/freedesktop/UPower", "org.freedesktop.UPower",
                                QDBusConnection::systemBus(), this);
    connect(uPower, SIGNAL(Changed()), this, SLOT(uPowerChange()));
    uPowerProperties = new QDBusInterface("org.freedesktop.UPower", "/org/freedesktop/UPower", "org.freedesktop.DBus.Properties",
                                          QDBusConnection::systemBus(), this);
}

void Lid::uPowerChange()
{
    bool newIsClosed = uPower->property("LidIsClosed").toBool();
    if (newIsClosed != closed)
    {
        closed = newIsClosed;
        emit changed(closed);
    }
}
