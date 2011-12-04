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


#ifndef QTXDG_XDGMENUWIDGET_H
#define QTXDG_XDGMENUWIDGET_H

#include <QMenu>
#include <QtXml/QDomElement>

class XdgMenu;
class QEvent;
class XdgMenuWidgetPrivate;


/*!
 @brief The XdgMenuWidget class provides an QMenu widget for application menu or its part.

 Example usage:
 @code
    QString menuFile = XdgMenu::getMenuFileName();
    XdgMenu xdgMenu(menuFile);

    bool res = xdgMenu.read();
    if (res)
    {
        XdgMenuWidget menu(xdgMenu, "", this);
        menu.exec(QCursor::pos());
    }
    else
    {
        QMessageBox::warning(this, "Parse error", xdgMenu.errorString());
    }
 @endcode
 */

class XdgMenuWidget : public QMenu
{
    Q_OBJECT
public:
    /// Constructs a menu for root documentElement in xdgMenu with some text and parent.
    XdgMenuWidget(const XdgMenu& xdgMenu, const QString& title="", QWidget* parent=0);

    /// Constructs a menu for menuElement with parent.
    explicit XdgMenuWidget(const QDomElement& menuElement, QWidget* parent=0);

    /// Constructs a copy of other.
    XdgMenuWidget(const XdgMenuWidget& other, QWidget* parent=0);

    /// Assigns other to this menu.
    XdgMenuWidget& operator=(const XdgMenuWidget& other);

    /// Destroys the menu.
    virtual ~XdgMenuWidget();

    /// @reimp
    QSize sizeHint() const;
signals:

protected:
    bool event(QEvent* event);

private:
    XdgMenuWidgetPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(XdgMenuWidget)
};

#endif // QTXDG_XDGMENUWIDGET_H
