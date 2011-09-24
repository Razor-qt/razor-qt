/* BEGIN_COMMON_COPYRIGHT_HEADER
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



#ifndef RAZORMOUNTCONFIGURATION_H
#define RAZORMOUNTCONFIGURATION_H


#include "../panel/razorpanelpluginconfigdialog.h"

namespace Ui {
    class RazorMountConfiguration;
}

class RazorMountConfiguration : public RazorPanelPluginConfigDialog
{
    Q_OBJECT

public:
    explicit RazorMountConfiguration(QSettings &settings, QWidget *parent = 0);
    ~RazorMountConfiguration();

protected slots:
    virtual void loadSettings();
    void devAddedChanged(int index);

private:
    Ui::RazorMountConfiguration *ui;
};

#endif // RAZORMOUNTCONFIGURATION_H
