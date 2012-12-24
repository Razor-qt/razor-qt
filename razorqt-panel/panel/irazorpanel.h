/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
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


#ifndef IRAZORPANEL_H
#define IRAZORPANEL_H
#include <QRect>

class IRazorPanelPlugin;

/**
 **/
class IRazorPanel
{
public:
    /**
    Specifies the position of the panel on screen.
    **/
    enum Position{
        PositionBottom, //! The bottom side of the screen.
        PositionTop,    //! The top side of the screen.
        PositionLeft,   //! The left side of the screen.
        PositionRight   //! The right side of the screen.
    };

    /**
     This property holds position of the panel.
     Possible values for this property are described by the Position enum
     **/
    virtual Position position() const = 0;

    virtual int lineSize() const = 0;
    virtual int lineCount() const = 0;

    /**
     Helper functions for eazy direction checking.
     Retuns true if panel on the top or bottom of the screen; otherwise returns false.
     **/
    bool isHorizontal() const { return position() == PositionBottom || position() == PositionTop; }

    /**
     Returns global screen coordinates of the panel. You no need to use mapToGlobal.
     **/
    virtual QRect globalGometry() const = 0;

    /**
     Helper functions for calculating global screen position of some popup window with windowSize size.
     If you need to show some popup window, you can use it, to get global screen position for the new window.
     **/
    virtual QRect calculatePopupWindowPos(const IRazorPanelPlugin *plugin, const QSize &windowSize) const = 0;
};

#endif // IRAZORPANEL_H
