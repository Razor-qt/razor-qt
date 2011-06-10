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


#ifndef XDGMENU_H
#define XDGMENU_H

#include "xdgdesktopfile.h"

#include <QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QHash>
#include <QtXml/QDomDocument>


class QDomDocument;
class QDomElement;
class XdgMenuPrivate;

/*! @brief The XdgMenu class implements the "Desktop Menu Specification" from freedesktop.org.

 Freedesktop menu is a user-visible hierarchy of applications, typically displayed as a menu.

 Example usage:
@code
    QString menuFile = XdgMenu::getMenuFileName();
    XdgMenu xdgMenu(menuFile);

    bool res = xdgMenu.read();
    if (!res)
    {
        QMessageBox::warning(this, "Parse error", xdgMenu.errorString());
    }

    QDomElement rootElement = xdgMenu.xml().documentElement()
 @endcode

 @sa http://specifications.freedesktop.org/menu-spec/menu-spec-latest.html
 */

class XdgMenu : public QObject
{
Q_OBJECT
public:
    explicit XdgMenu(const QString& menuFileName, QObject *parent = 0);
    virtual ~XdgMenu();

    bool read();
    void save(const QString& fileName);

    const QDomDocument xml() const;
    QString menuFileName() const;

    QDomElement findMenu(QDomElement& baseElement, const QString& path, bool createNonExisting);

    /// A list of strings identifying the environments that should display a desktop entry.
    QStringList& environments();

    const QString errorString() const;

    const QString logDir() const;
    void setLogDir(const QString& directory);

    static QString getMenuFileName(const QString& baseName = "applications.menu");

private:
    XdgMenuPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(XdgMenu)
};


#endif // XDGMENU_H
