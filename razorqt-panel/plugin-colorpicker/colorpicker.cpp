/* BEGIN_COMMON_COPYRIGHT_HEADER
    * (c)LGPL2+
    *
    * Razor - a lightweight, Qt based, desktop toolset
    * http://razor-qt.org
    *
    * Copyright: 2012 Razor team
    * Authors:
    *   Aaron Lewis <the.warl0ck.1989@gmail.com>
    *
    * This program or library is free software; you can redistribute it
    * and/or modify it under the terms of the GNU Lesser General Public
    * License as published by the Free Software Foundation; either
    * version 2.1 of the License, or (at your option) any later version.
    *
    * This library is distributed in the hope that it will be useful,
    * but WITHOUT ANY WARRANTY; without even the implied warranty of
    * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    * Lesser General Public License for more details.

    * You should have received a copy of the GNU Lesser General
    * Public License along with this library; if not, write to the
    * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    * Boston, MA 02110-1301 USA
    *
    * END_COMMON_COPYRIGHT_HEADER */
#include "colorpicker.h"

EXPORT_RAZOR_PANEL_PLUGIN_CPP (ColorPicker)

ColorPicker::ColorPicker(const RazorPanelPluginStartInfo* startInfo, QWidget* parent):
        RazorPanelPlugin (startInfo , parent)
{
    setObjectName ("ColorPicker");

    QFontMetrics fm (font());
    lineEdit.setFixedWidth ( 10*fm.width ("a") );

    layout()->addWidget (&button);
    layout()->addWidget (&lineEdit);

    button.setIcon ( XdgIcon::fromTheme("kcolorchooser") );

    connect (&button , SIGNAL(clicked()) , &thread , SLOT(start()));
    connect (&thread , SIGNAL(started()) , SLOT(loop()));
}

ColorPicker::~ColorPicker()
{
}

void ColorPicker::loop ()
{
    Display *d;
    Window root;

    d = XOpenDisplay(NULL);
    if ( ! d )
        return;

    root = DefaultRootWindow(d);

    XGrabPointer(d, root, False, ButtonPressMask | ButtonReleaseMask |
            PointerMotionMask, GrabModeAsync, GrabModeAsync, None,
            None, CurrentTime);

    XEvent ev;
    while (1) {
        XNextEvent(d, &ev);

        if (ev.type == ButtonRelease)
        {
            QApplication::processEvents();
            takeScreenshot();
            break;
        }
    }

    XCloseDisplay(d);

    thread.terminate();
}

void ColorPicker::takeScreenshot ()
{
    const QPoint & pos = QCursor::pos();
    QPixmap pixmap = QPixmap::grabWindow(QApplication::desktop()->winId(), pos.x(), pos.y(), 1, 1);

    QImage img = pixmap.toImage();
    QColor col = QColor(img.pixel(0,0));

    lineEdit.setText (col.name());
}
