/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
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

#ifndef POWERMANAGER_H
#define POWERMANAGER_H

#include <QObject>
#include <QAction>

class RazorPower;

/*! QAction centric menu aware wrapper around razorpower
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
    // power management
    void suspend();
    void hibernate();
    void reboot();
    void shutdown();
    // razor session
    void logout();

private:
    RazorPower * m_power;
    QWidget * m_parentWidget;

private slots:
    void hibernateFailed();
    void suspendFailed();
};

#endif
