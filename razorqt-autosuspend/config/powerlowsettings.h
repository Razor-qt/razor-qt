/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Christian Surlykke <christian@surlykke.dk>
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
#ifndef POWERLOWSETTINGS_H
#define POWERLOWSETTINGS_H

#include <QWidget>
#include <razorqt/razorsettings.h>

namespace Ui {
class PowerLowSettings;
}

class PowerLowSettings : public QWidget
{
    Q_OBJECT
    
public:
    explicit PowerLowSettings(RazorSettings *settings, QWidget *parent = 0);
    ~PowerLowSettings();

public slots:
    void loadSettings();

private slots:
    void saveSettings();

private:
    RazorSettings *m_Settings;
    Ui::PowerLowSettings *ui;
    bool loading;
};

#endif // POWERLOWSETTINGS_H
