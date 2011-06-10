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


#include "xdgmenuwidget.h"
#include <razorqt/xdgicon.h>
#include <razorqt/domhelper.h>
#include <razorqt/xdgaction.h>
#include "xdgmenu.h"
#include <QtCore/QEvent>
#include <QDebug>


class XdgMenuWidgetPrivate
{
private:
    XdgMenuWidget* const q_ptr;
    Q_DECLARE_PUBLIC(XdgMenuWidget);

public:
    explicit XdgMenuWidgetPrivate(XdgMenuWidget* parent):
        q_ptr(parent),
        mNeedBuild(true)
    {}

    void init(const QDomElement& xml);
    void buildMenu();


    QDomElement mXml;
    bool mNeedBuild;

private:
    XdgAction* createAction(const QDomElement& xml);
};



/************************************************

 ************************************************/
XdgMenuWidget::XdgMenuWidget(const XdgMenu& xdgMenu, const QString& title, QWidget* parent):
    QMenu(parent),
    d_ptr(new XdgMenuWidgetPrivate(this))
{
    d_ptr->init(xdgMenu.xml().documentElement());
    setTitle(title);
}


/************************************************

 ************************************************/
XdgMenuWidget::XdgMenuWidget(const QDomElement& menuElement, QWidget* parent):
    QMenu(parent),
    d_ptr(new XdgMenuWidgetPrivate(this))
{
    d_ptr->init(menuElement);
}


/************************************************

 ************************************************/
XdgMenuWidget::XdgMenuWidget(const XdgMenuWidget& other, QWidget* parent):
    QMenu(parent),
    d_ptr(new XdgMenuWidgetPrivate(this))
{
    d_ptr->init(other.d_ptr->mXml);
}


/************************************************

 ************************************************/
void XdgMenuWidgetPrivate::init(const QDomElement& xml)
{
    Q_Q(XdgMenuWidget);
    mXml = xml;

    q->clear();
    mNeedBuild = true;

    if (! xml.attribute("title").isEmpty())
        q->setTitle(xml.attribute("title"));
    else
        q->setTitle(xml.attribute("name"));

    q->setToolTip(xml.attribute("comment"));


    QIcon parentIcon;
    QMenu* parentMenu = qobject_cast<QMenu*>(q->parent());
    if (parentMenu)
        parentIcon = parentMenu->icon();

    q->setIcon(XdgIcon::fromTheme(mXml.attribute("icon"), parentIcon));

    if (!qobject_cast<XdgMenuWidget*>(q->parent()))
        buildMenu();
}


/************************************************

 ************************************************/
XdgMenuWidget::~XdgMenuWidget()
{
    delete d_ptr;
}


/************************************************

 ************************************************/
XdgMenuWidget& XdgMenuWidget::operator=(const XdgMenuWidget& other)
{
    Q_D(XdgMenuWidget);
    d->init(other.d_ptr->mXml);

    return *this;
}


/************************************************

 ************************************************/
bool XdgMenuWidget::event(QEvent* event)
{
    if (event->type() == QEvent::Show)
    {
        Q_D(XdgMenuWidget);
        if (d->mNeedBuild)
            d->buildMenu();
    }
    return QMenu::event(event);
}


/************************************************

 ************************************************/
void XdgMenuWidgetPrivate::buildMenu()
{
    Q_Q(XdgMenuWidget);

    QAction* first = 0;
    if (!q->actions().isEmpty())
        first = q->actions().last();


    DomElementIterator it(mXml, "");
    while(it.hasNext())
    {
        QDomElement xml = it.next();

        // Build submenu ........................
        if (xml.tagName() == "Menu")
            q->insertMenu(first, new XdgMenuWidget(xml, q));

        //Build application link ................
        else if (xml.tagName() == "AppLink")
            q->insertAction(first, createAction(xml));
    }

    mNeedBuild = false;
}



/************************************************

 ************************************************/
XdgAction* XdgMenuWidgetPrivate::createAction(const QDomElement& xml)
{
    Q_Q(XdgMenuWidget);
    XdgAction* action = new XdgAction(xml.attribute("desktopFile"), q);

    QString title;
    if (!xml.attribute("title").isEmpty())
        title = xml.attribute("title");
    else
        title = xml.attribute("name");


    if (!xml.attribute("genericName").isEmpty())
        title += QString(" (%1)").arg(xml.attribute("genericName"));

    action->setText(title);
    return action;
}


/************************************************

 ************************************************/
QSize XdgMenuWidget::sizeHint() const
{
    Q_D(const XdgMenuWidget);
    if (d->mNeedBuild)
    {
        XdgMenuWidgetPrivate* p = const_cast<XdgMenuWidgetPrivate*>(d);
        p->buildMenu();
    }
    return QMenu::sizeHint();
}
