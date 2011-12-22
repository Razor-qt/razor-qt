/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL3+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
 *   Chris "VdoP" Regali
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
 
#ifndef XDGAUTOSTART_H
#define XDGAUTOSTART_H


//2010 Chris "VdoP" Regali


/**
 * @file xdgautostart.h
 * @brief Declares the class xdgautostart
 * @date 9.9.2010 (DDMMYYYY)
 * @author Christopher "VdoP" Regali
  **/

#include <qtxdg/xdgdesktopfile.h>

/**
 * @brief manages the xdg-correct autostart stuff
 **/

class XdgAutoStart
{
public:
    XdgAutoStart();
    ~XdgAutoStart();
    QList<XdgDesktopFile*> list();

private:
    QList<XdgDesktopFile*> m_list;
    QStringList badNames;

    void updateList();
    void addDirtoList(const QString & _dir);
    void debugAutostart();
    void cleanList();
};


#endif
