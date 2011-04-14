/********************************************************************
  Copyright: 2011 Alexander Sokoloff <sokoloff.a@gmail.ru>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License.
  version 2 as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*********************************************************************/

#ifndef XDGMENUWIDGET_H
#define XDGMENUWIDGET_H

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

#endif // XDGMENUWIDGET_H
