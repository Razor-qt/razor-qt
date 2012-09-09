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


#ifndef RAZORPOWER_PROVIDERS_H
#define RAZORPOWER_PROVIDERS_H

#include <QtCore/QObject>
#include <razorqt/razorsettings.h>
#include "razorpower.h"

class RazorPowerProvider: public QObject
{
    Q_OBJECT
public:
    
    enum DbusErrorCheck {
        CheckDBUS,
        DontCheckDBUS
    };
    
    explicit RazorPowerProvider(QObject *parent = 0);
    virtual ~RazorPowerProvider();

    /*! Returns true if the RazorPower can perform action.
        This is a pure virtual function, and must be reimplemented in subclasses. */
    virtual bool canAction(RazorPower::Action action) const = 0 ;

public slots:
    /*! Performs the requested action.
        This is a pure virtual function, and must be reimplemented in subclasses. */
    virtual bool doAction(RazorPower::Action action) = 0;
};


class UPowerProvider: public RazorPowerProvider
{
    Q_OBJECT
public:
    UPowerProvider(QObject *parent = 0);
    ~UPowerProvider();
    bool canAction(RazorPower::Action action) const;

public slots:
    bool doAction(RazorPower::Action action);
};


class ConsoleKitProvider: public RazorPowerProvider
{
    Q_OBJECT
public:
    ConsoleKitProvider(QObject *parent = 0);
    ~ConsoleKitProvider();
    bool canAction(RazorPower::Action action) const;

public slots:
    bool doAction(RazorPower::Action action);
};


class RazorProvider: public RazorPowerProvider
{
    Q_OBJECT
public:
    RazorProvider(QObject *parent = 0);
    ~RazorProvider();
    bool canAction(RazorPower::Action action) const;

public slots:
    bool doAction(RazorPower::Action action);
};


class HalProvider: public RazorPowerProvider
{
    Q_OBJECT
public:
    HalProvider(QObject *parent = 0);
    ~HalProvider();
    bool canAction(RazorPower::Action action) const;

public slots:
    bool doAction(RazorPower::Action action);
};

class CustomProvider: public RazorPowerProvider
{
    Q_OBJECT
public:
    CustomProvider(QObject *parent = 0);
    ~CustomProvider();
    bool canAction(RazorPower::Action action) const;

public slots:
    bool doAction(RazorPower::Action action);

private:
    RazorSettings mSettings;
};

#endif // RAZORPOWER_PROVIDERS_H
