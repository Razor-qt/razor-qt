/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Kuzma Shapran <kuzma.shapran@gmail.com>
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

#ifndef RAZORPANELKBINDICATOR_H
#define RAZORPANELKBINDICATOR_H

#include "../panel/razorpanelplugin.h"
#include "razorkbindicatorconfiguration.h"


class QLabel;

namespace KbIndicator {
    class BaseStat;
}

class RazorKbIndicator : public RazorPanelPlugin
{
    Q_OBJECT
public:
    RazorKbIndicator(const RazorPanelPluginStartInfo* startInfo, QWidget *parent = NULL);
    ~RazorKbIndicator();

    virtual RazorPanelPlugin::Flags flags() const { return PreferRightAlignment | HaveConfigDialog; }

protected slots:
    virtual void settingsChanged();
    virtual void showConfigureDialog();

protected:
//    void paintEvent(QPaintEvent *);

private:
    QLabel *content;

    int bit;
};


EXPORT_RAZOR_PANEL_PLUGIN_H

#endif // RAZORPANELKBINDICATOR_H
