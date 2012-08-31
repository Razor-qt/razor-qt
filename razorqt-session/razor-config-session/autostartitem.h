/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright (C) 2012  Alec Moskvin <alecm@gmx.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#ifndef AUTOSTARTITEM_H
#define AUTOSTARTITEM_H

#include <qtxdg/xdgdesktopfile.h>

/*! \brief The AutostartItem class provides an interface for staging configuration ofindividual
autostart items. All changes are made in memory until commit() is called.

 - "system" file refers to a read-only file in /etc/xdg/autostart (or a directory in $XDG_CONFIG_DIRS)
 - "local" file refers to the file in user's ~/.config/autostart (or in $XDG_CONFIG_HOME)

When a "local" file has the same name as the "system" file, the local one overrides it. This class
tries to ensure that the "local" file is deleted if it's the same as the "system" file.
*/

class AutostartItem
{
public:
    //! \brief Default constructor
    AutostartItem();

    /*! Creates an autostart item with a "system" file
     * \param systemFile
     */
    AutostartItem(const XdgDesktopFile& systemFile);

    //! Destructor
    ~AutostartItem();

    //! Returns the "active" desktop file
    const XdgDesktopFile& file() const;

    //! Returns the "system" file
    const XdgDesktopFile& systemfile() const;

    /*! Sets the local version to the specified desktop file. Use this to make modifications.
     * \param file the desktop file
     * \param real set to true when setting the local file from disk
     */
    void setLocal(const XdgDesktopFile& file, bool real=false);

    /*! Sets whether the item auto-starts
     * \param enable When false, sets the "Hidden" key which will prevent the item from starting
     */
    void setEnabled(bool enable);

    /*! Removes the "local" version of the file
     * \return true if there is no "system" verion left
     */
    bool removeLocal();

    //! Returns true if the item will auto-start
    bool isEnabled() const;

    //! Returns true if both the "local" and "system" versions exist
    bool overrides() const { return system && local; }

    //! Returns true if the local version exists
    bool isLocal() const { return local; }

    //! Returns true if the local file does not exist on disk
    bool isTransient() const { return state == TRANSIENT; }

    //! Returns true if this object carries no useful information and can be ignored/deleted
    bool isEmpty() const;

    /*! Write the changes to disk
     * \return true on success
     */
    bool commit();

    //! Creates a mapping of file name (a unique ID) to AutostartItem for the running system
    static QMap<QString, AutostartItem> createItemMap();

private:
    XdgDesktopFile systemFile;
    XdgDesktopFile localFile;
    enum ItemState
    {
        DEFAULT,
        MODIFIED,  // local file was modified
        TRANSIENT, // local file does not exist on disk
        DELETED    // local file needs to be deleted from disk
    } state;
    bool system;   // system file is set
    bool local;    // local file is set
};

#endif // AUTOSTARTITEM_H
