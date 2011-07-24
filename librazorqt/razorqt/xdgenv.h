/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * https://sourceforge.net/projects/razor-qt/
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.ru>
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


#ifndef XDGENV_H
#define XDGENV_H

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <stdlib.h>

/*! @brief The XdgMenu class implements the "XDG Base Directory Specification" from freedesktop.org.
 * This specification defines where these files should be looked for by defining one or more base
 * directories relative to which files should be located.
 *
 * @sa http://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html
 */
class XdgEnv
{
public:
    /*! @brief Returns the path to the directory that corresponds to the $XDG_DATA_HOME.
     * If @i createDir is true, the function will create the directory.
     *
     * $XDG_DATA_HOME defines the base directory relative to which user specific data files
     * should be stored. If $XDG_DATA_HOME is either not set or empty, a default equal to
     * $HOME/.local/share should be used.
     */
    static QString dataHome(bool createDir=true);


    /*! @brief Returns the path to the directory that corresponds to the $XDG_CONFIG_HOME.
     * If @i createDir is true, the function will create the directory.
     *
     * $XDG_CONFIG_HOME defines the base directory relative to which user specific configuration
     * files should be stored. If $XDG_CONFIG_HOME is either not set or empty, a default equal
     * to $HOME/.config should be used.
     */
    static QString configHome(bool createDir=true);


    /*! @brief Returns a list of all directories that corresponds to the $XDG_DATA_DIRS.
     * $XDG_DATA_DIRS defines the preference-ordered set of base directories to search for data
     * files in addition to the $XDG_DATA_HOME base directory. If $XDG_DATA_DIRS is either not set
     * or empty, a value equal to /usr/local/share/:/usr/share/ should be used.
     */
    static QStringList dataDirs();



    /*! @brief Returns a list of all directories that corresponds to the $XDG_CONFIG_DIRS.
     * $XDG_CONFIG_DIRS defines the preference-ordered set of base directories to search for
     * configuration files in addition to the $XDG_CONFIG_HOME base directory. If $XDG_CONFIG_DIRS
     * is either not set or empty, a value equal to /etc/xdg should be used.
     */
    static QStringList configDirs();


    /*! @brief Returns the path to the directory that corresponds to the $XDG_CACHE_HOME.
     * If @i createDir is true, the function will create the directory.
     *
     * $XDG_CACHE_HOME defines the base directory relative to which user specific non-essential
     * data files should be stored. If $XDG_CACHE_HOME is either not set or empty,
     * a default equal to $HOME/.cache should be used.
     */
    static QString cacheHome(bool createDir=true);


    /*! @brief Returns the path to the directory that corresponds to the $XDG_RUNTIME_DIR.
     * $XDG_RUNTIME_DIR defines the base directory relative to which user-specific non-essential
     * runtime files and other file objects (such as sockets, named pipes, ...) should be stored.
     * The directory MUST be owned by the user, and he MUST be the only one having read and write
     * access to it. Its Unix access mode MUST be 0700.
     */
     static QString runtimeDir();
};

#endif // XDGENV_H
