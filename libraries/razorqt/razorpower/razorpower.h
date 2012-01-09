/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */


#ifndef RAZORPOWER_H
#define RAZORPOWER_H

#include <QtCore/QObject>
#include <QtCore/QList>

class RazorPowerProvider;

/*! RazorPower class provides an interface to control system-wide power and session management.
    It allows logout from the user session, hibernate, reboot, shutdown and suspend computer.
    This is a wrapper class. All the real work is done in the RazorPowerWorker classes.
*/
class RazorPower : public QObject
{
    Q_OBJECT
public:
    /// RazorPower can perform next actions:
    enum Action{
        PowerLogout,    /// Close the current user session.
        PowerHibernate, /// Hibernate the comuter
        PowerReboot,    /// Reboot the comuter
        PowerShutdown,  /// Shutdown the comuter
        PowerSuspend    /// Suspend the comuter
    };

    /// Constructs a RazorPower with parent.
    explicit RazorPower(QObject *parent = 0);

    /// Destroys the object.
    virtual ~RazorPower();

    /// Returns true if the RazorPower can perform action.
    bool canAction(Action action) const;

    //! This function is provided for convenience. It's equivalent to calling canAction(PowerLogout).
    bool canLogout() const;

    //! This function is provided for convenience. It's equivalent to calling canAction(PowerHibernate).
    bool canHibernate() const;

    //! This function is provided for convenience. It's equivalent to calling canAction(PowerReboot).
    bool canReboot() const;

    //! This function is provided for convenience. It's equivalent to calling canAction(PowerShutdown).
    bool canShutdown() const;

    //! This function is provided for convenience. It's equivalent to calling canAction(PowerSuspend).
    bool canSuspend() const;

public slots:
    /// Performs the requested action.
    bool doAction(Action action);

    //! This function is provided for convenience. It's equivalent to calling doAction(PowerLogout).
    bool logout();

    //! This function is provided for convenience. It's equivalent to calling doAction(PowerHibernate).
    bool hibernate();

    //! This function is provided for convenience. It's equivalent to calling doAction(PowerReboot).
    bool reboot();

    //! This function is provided for convenience. It's equivalent to calling doAction(PowerShutdown).
    bool shutdown();

    //! This function is provided for convenience. It's equivalent to calling doAction(PowerSuspend).
    bool suspend();

private:
    QList<RazorPowerProvider*> mProviders;
};

#endif // RAZORPOWER_H
