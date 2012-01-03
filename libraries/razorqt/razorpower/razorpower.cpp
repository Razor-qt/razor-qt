/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.ru>
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


#include "razorpower.h"
#include "razorpowerproviders.h"
#include <QtCore/QtAlgorithms>
#include <QtCore/QDebug>

RazorPower::RazorPower(QObject *parent) :
    QObject(parent)
{
    mProviders.append(new UPowerProvider(this));
    mProviders.append(new ConsoleKitProvider(this));
    mProviders.append(new RazorProvider(this));
}


RazorPower::~RazorPower()
{
}


bool RazorPower::canAction(RazorPower::Action action) const
{
    foreach(RazorPowerProvider* provider, mProviders)
    {
        if (provider->canAction(action))
            return true;
    }

    return false;
}


bool RazorPower::doAction(RazorPower::Action action)
{
    foreach(RazorPowerProvider* provider, mProviders)
    {
        if (provider->canAction(action) &&
            provider->doAction(action)
           )
        {
            return true;
        }
    }
    return false;
}


bool RazorPower::canLogout()    const { return canAction(PowerLogout);    }
bool RazorPower::canHibernate() const { return canAction(PowerHibernate); }
bool RazorPower::canReboot()    const { return canAction(PowerReboot);    }
bool RazorPower::canShutdown()  const { return canAction(PowerShutdown);  }
bool RazorPower::canSuspend()   const { return canAction(PowerSuspend);   }

bool RazorPower::logout()       { return doAction(PowerLogout);    }
bool RazorPower::hibernate()    { return doAction(PowerHibernate); }
bool RazorPower::reboot()       { return doAction(PowerReboot);    }
bool RazorPower::shutdown()     { return doAction(PowerShutdown);  }
bool RazorPower::suspend()      { return doAction(PowerSuspend);   }
