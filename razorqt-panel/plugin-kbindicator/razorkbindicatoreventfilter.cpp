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

#include "razorkbindicatoreventfilter.h"

#include <X11/XKBlib.h>


static RazorKbIndicatorEventFilter *filterObject = NULL;


bool X11_eventFilter(void *message)
{
    return filterObject ? filterObject->X11_eventFilter(message) : false;
}

RazorKbIndicatorEventFilter* RazorKbIndicatorEventFilter::instance(void)
{
    if (!filterObject)
        new RazorKbIndicatorEventFilter();
    return filterObject;
}

RazorKbIndicatorEventFilter::RazorKbIndicatorEventFilter(QObject *parent):
    QObject(parent)
{
    if (!filterObject)
    {
        filterObject = this;

        display = QX11Info::display();

        if (initXkbExtension())
            if (XkbSelectEvents(display, XkbUseCoreKbd, XkbIndicatorStateNotifyMask, XkbIndicatorStateNotifyMask))
                oldEventFilter = QAbstractEventDispatcher::instance()->setEventFilter(::X11_eventFilter);
    }
}

bool RazorKbIndicatorEventFilter::initXkbExtension(void)
{
    int code;
    int major = XkbMajorVersion;
    int minor = XkbMinorVersion;
    int XkbErrorBase;

    if (!XkbLibraryVersion(&major, &minor))
        return false;
    if (!XkbQueryExtension(display, &code, &XkbEventBase, &XkbErrorBase, &major, &minor))
        return false;
    if (!XkbUseExtension(display, &major, &minor))
        return false;
    return true;
}

bool RazorKbIndicatorEventFilter::X11_eventFilter(void *message)
{
    XkbEvent* event = static_cast<XkbEvent*>(message);

    if (event->type == XkbEventBase + XkbEventCode)
        if (event->any.xkb_type == XkbIndicatorStateNotify)
            emit indicatorsChanged(event->indicators.changed, event->indicators.state);

    if (oldEventFilter)
        return oldEventFilter(message);

    return false;
}

bool RazorKbIndicatorEventFilter::getLockStatus(int bit)
{
    bool state = false;
    if (display)
    {
        unsigned n;
        XkbGetIndicatorState(display, XkbUseCoreKbd, &n);
        state = (n & (1 << bit));
    }
    return state;
}
