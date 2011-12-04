/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
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
    QWidget* parentWidget() const { return m_parentWidget; }
    void setParentWidget(QWidget* parentWidget) { m_parentWidget = parentWidget; }
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
    QWidget * m_parentWidget;

private slots:
    void hibernateFailed();
    void suspendFailed();
    void monitoring(const QString & msg);
};

#endif
