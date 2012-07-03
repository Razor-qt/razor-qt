/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Aaron Lewis <the.warl0ck.1989@gmail.com>
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
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#ifndef RAZOR_COLORPICKER_H
#define RAZOR_COLORPICKER_H

#include <QtGui/QApplication>
#include <QtGui/QFontMetrics>
#include <QtGui/QDesktopWidget>
#include <QtGui/QLineEdit>
#include <QtGui/QToolButton>

#include "../panel/razorpanelplugin.h"
#include <qtxdg/xdgicon.h>

class ColorPicker : public RazorPanelPlugin
{
    Q_OBJECT
public:
    ColorPicker(const RazorPanelPluginStartInfo* startInfo, QWidget* parent = 0);
    ~ColorPicker();

protected:
    void mouseReleaseEvent(QMouseEvent *event);

private slots:
    void captureMouse();

private:
    QLineEdit lineEdit;
    QToolButton button;
    bool mCapturing;
};

EXPORT_RAZOR_PANEL_PLUGIN_H

#endif
