/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation;  only version 2 of
 * the License is valid for this program.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
    void infoMessage();
    void monitoring(const QString & msg);
};

#endif
