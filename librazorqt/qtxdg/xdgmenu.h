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



#ifndef QTXDG_XDGMENU_H
#define QTXDG_XDGMENU_H

#include <QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtXml/QDomDocument>


class QDomDocument;
class QDomElement;
class XdgMenuPrivate;


/*! @brief The XdgMenu class implements the "Desktop Menu Specification" from freedesktop.org.

 Freedesktop menu is a user-visible hierarchy of applications, typically displayed as a menu.

 Example usage:
@code
    QString menuFile = XdgMenu::getMenuFileName();
    XdgMenu xdgMenu();

    bool res = xdgMenu.read(menuFile);
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
    friend class XdgMenuReader;
    friend class XdgMenuApplinkProcessor;

public:
    explicit XdgMenu(QObject *parent = 0);
    virtual ~XdgMenu();

    bool read(const QString& menuFileName);
    void save(const QString& fileName);

    const QDomDocument xml() const;
    QString menuFileName() const;

    QDomElement findMenu(QDomElement& baseElement, const QString& path, bool createNonExisting);

    /// A list of strings identifying the environments that should display a desktop entry.
    QStringList environments();
    /// Set currently running environments. Example: RAZOR, KDE, or GNOME...
    void setEnvironments(const QStringList &envs);
    void setEnvironments(const QString &env);

    /*!
     * Returns a string description of the last error that occurred if read() returns false.
     */
    const QString errorString() const;

    /*!
     * @brief The name of the directory for the debug XML-files.
     */
    const QString logDir() const;

    /*!
     * @brief The name of the directory for the debug XML-files. If a directory is specified,
     * then after you run the XdgMenu::read, you can see and check the results of the each step.
     */
    void setLogDir(const QString& directory);

    static QString getMenuFileName(const QString& baseName = "applications.menu");

    bool isOutDated() const;

protected:
    void addWatchPath(const QString& path);

private:
    XdgMenuPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(XdgMenu)
};


#endif // QTXDG_XDGMENU_H
