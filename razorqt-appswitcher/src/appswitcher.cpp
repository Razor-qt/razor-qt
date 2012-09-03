/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
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

// include order is important here...
#include "appswitcher.h"

#include <QtCore/QtDebug>
#include <QtCore/QEvent>
#include <QtCore/QTimer>
#include <QtGui/QDesktopWidget>
#include <QtGui/QKeyEvent>
#include <QtGui/QMessageBox>
#include <QtGui/QShortcut>
#include <QtGui/QHBoxLayout>

#include <razorqt/xfitman.h>
#include <qtxdg/xdgicon.h>
#include <razorqt/razorsettings.h>
#include <razorqxt/qxtglobalshortcut.h>
#include <razorqt/razornotification.h>


#define DEFAULT_SHORTCUT "Alt+Tab"


RazorAppSwitcher::AppSwitcher::AppSwitcher()
    : QWidget(0, Qt::FramelessWindowHint | Qt::Tool)
{
    setupUi(this);

    m_settings = new RazorSettings("appswitcher", this);

    installEventFilter(this);

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(close()));
    m_timer->setInterval(10000);

    m_layout = new QHBoxLayout();
    QWidget * background = new QWidget(this);
    scrollArea->setWidget(background);
    background->setLayout(m_layout);

    m_key = new QxtGlobalShortcut(this);

    connect(m_settings, SIGNAL(settingsChanged()), this, SLOT(applySettings()));
    connect(m_key, SIGNAL(activated()), this, SLOT(globalKeyActivated()));
    
    applySettings();
}

void RazorAppSwitcher::AppSwitcher::applySettings()
{
    QKeySequence shortcut = QKeySequence::fromString(m_settings->value("shortcut", DEFAULT_SHORTCUT).toString());
    if (shortcut.isEmpty())
        shortcut = QKeySequence::fromString(DEFAULT_SHORTCUT);
    
    if (m_key->shortcut() == shortcut)
        return;

    if (! m_key->setShortcut(shortcut))
    {
        RazorNotification::notify(tr("Global shorcut: '%1' cannot be registered").arg(shortcut.toString()));
        exit(1);
    }
    
    m_settings->sync();
}

void RazorAppSwitcher::AppSwitcher::globalKeyActivated()
{
    if (isVisible())
        selectNextItem();
    else
        handleApps();
}

void RazorAppSwitcher::AppSwitcher::handleApps()
{
//    qDebug() << "RazorAppSwitcher::AppSwitcher::handleApps()";

    if (m_layout->count())
    {
        QLayoutItem * item;
        while ((item = m_layout->takeAt(0)) != 0)
        {
            item->widget()->hide();
            delete item;
        }
    }
    m_list.clear();

    QList<Window> l = xfitMan().getClientList();
    QList<Window> merge;
    // setup already used windows
    foreach (Window w, m_orderedWindows)
    {
        if (l.contains(w))
        {
            merge.append(w);
            l.removeAll(w);
        }
        else
            m_orderedWindows.removeAll(w);
    }
    // rest at the end
    merge += l;

    // setup new windows
    foreach (Window w, merge)
    {
        if (!xfitMan().acceptWindow(w))
        {
            continue;
        }
        QPixmap pm;
        if (! xfitMan().getClientIcon(w, pm))
            qDebug() << "No icon for:" << w << xfitMan().getWindowTitle(w);

        SwitcherItem * item = new SwitcherItem(w, xfitMan().getWindowTitle(w), pm, this);
        connect(item, SIGNAL(infoChanged(const QString&)),
                infoLabel, SLOT(setText(const QString&)));
        connect(item, SIGNAL(infoChanged(const QString&)),
                this, SLOT(setScrollAreaVisibility()));
        connect(item, SIGNAL(activateXWindow()), this, SLOT(activateXWindow()));
        m_list.append(item);
        m_layout->addWidget(item);
    }

    selectNextItem();

    // Warning: show() has to be called *after* setting focus. Dunno why
    // but it works now.
    if (!isVisible())
    {
        QRect desktop = QApplication::desktop()->availableGeometry(xfitMan().getActiveDesktop());
        int x, y;
        x = desktop.width()/2 - width() / 2;
        y = desktop.height()/2 - height() / 2;
        move(x, y);
        show();
    }

}

void RazorAppSwitcher::AppSwitcher::activateXWindow()
{
    SwitcherItem * item = qobject_cast<SwitcherItem*>(QApplication::focusWidget());
    if (! item)
    {
        qDebug() << "AppSwitcher::activateXWindow activation. Something is wrong - focus widget is not on SwitcherItem!";
        return;
    }
    int winDesktop = xfitMan().getWindowDesktop(item->window());
    if (xfitMan().getActiveDesktop() != winDesktop)
        xfitMan().setActiveDesktop(winDesktop);

    xfitMan().raiseWindow(item->window());
    close();
}

void RazorAppSwitcher::AppSwitcher::setScrollAreaVisibility()
{
    SwitcherItem *item = qobject_cast<SwitcherItem*>(sender());
    if (!item)
    {
        qDebug() << "setScrollAreaVisibility: signal sender is not SwitcherItem. And it's strange";
        return;
    }
    scrollArea->ensureWidgetVisible(item);
}

void RazorAppSwitcher::AppSwitcher::hideEvent(QHideEvent *e)
{
    m_timer->stop();
    QWidget::hideEvent(e);
}

void RazorAppSwitcher::AppSwitcher::showEvent(QShowEvent *e)
{
    m_timer->start();
    QWidget::showEvent(e);
}

void RazorAppSwitcher::AppSwitcher::keyPressEvent(QKeyEvent * e)
{
//    qDebug() << "RazorAppSwitcher::AppSwitcher::keyPressEvent()" << e;
    switch (e->key())
    {
        case Qt::Key_Escape:
            close();
            break;
        case Qt::Key_Enter:
        case Qt::Key_Return:
        {
            activateXWindow();
            break;
        }
    };

    QWidget::keyPressEvent(e);
}

void RazorAppSwitcher::AppSwitcher::keyReleaseEvent(QKeyEvent * e)
{
//    qDebug() << "AppSwitcher::keyReleaseEvent" << e << e->modifiers();
    // close window if there is no modifier pressed.
    // Here I assume that the key shortcut is always with ctrl or alt
    if (e->modifiers() == 0)
    {
        activateXWindow();
        close();
    }
    QWidget::keyReleaseEvent(e);
}

bool RazorAppSwitcher::AppSwitcher::eventFilter(QObject * o, QEvent * e)
{
//    qDebug() << "AppSwitcher::eventFilter" << e;
    if (e->type() == QEvent::WindowDeactivate)
    {
        close();
        return true;
    }
    return QWidget::eventFilter(o, e);
}

bool RazorAppSwitcher::AppSwitcher::handleEvent(XEvent * e)
{
    if (e->type == PropertyNotify)
    {
        if (!this)
            return false;

        Window w = xfitMan().getActiveWindow();
        if (m_orderedWindows.contains(w))
        {
            int ix = m_orderedWindows.indexOf(w);
            if (ix != -1)
            {
                m_orderedWindows.removeAt(ix);
            }
        }
        m_orderedWindows.prepend(w);
    }
    return false;
}

void RazorAppSwitcher::AppSwitcher::selectNextItem()
{
//    qDebug() << "RazorAppSwitcher::AppSwitcher::selectNextItem()";
    if (m_list.count() == 0)
    {
//        qDebug() << "AppSwitcher::selectItem m_list is empty. No action";
        return;
    }

    SwitcherItem * item = qobject_cast<SwitcherItem*>(QApplication::focusWidget());
    if (! item)
    {
        if (m_list.count() > 1)
        {
//            qDebug() << "AppSwitcher::selectItem implicit item (1) ";
//            qDebug() << m_list;
            item = m_list.at(1);
        }
        else
        {
//            qDebug() << "AppSwitcher::selectItem implicit item (0) ";
            item = m_list.at(0);
        }
    }
    else
    {
//        qDebug() << "AppSwitcher::selectItem selected" << item << item->text();
        int index = m_list.indexOf(item);
        if (index == -1)
        {
            qDebug() << "AppSwitcher::selectItem item not found in list. And it's wrong!" << item << item->text();
            index = 0;
        }
        ++index;
        if (index > m_list.count() - 1)
            index = 0;
        item = m_list.at(index);
    }

    item->setFocus(Qt::TabFocusReason);
}


RazorAppSwitcher::SwitcherItem::SwitcherItem(Window window, const QString & text,
                                             const QPixmap & pixmap, QWidget * parent)
    : QToolButton(parent),
      m_window(window)
{
    setObjectName("SwitcherItem '" + text + "'");
    //setFocusPolicy(Qt::WheelFocus);
    // needed for focus changing by mouse (macosx like)
    setMouseTracking(true);

    setMaximumSize(64, 64);
    setMinimumSize(64, 64);
    setToolButtonStyle(Qt::ToolButtonIconOnly);
    QSize sz(50, 50);
    setIconSize(sz);
    setToolTip(text);

    // enforce unified size of icons
    if (pixmap.size().height() != sz.height() || pixmap.size().width() != sz.width())
    {
        QPixmap pm = pixmap.scaled(sz, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        setIcon(pm);
    }
    else
        setIcon(pixmap);

    setText(text);
}

void RazorAppSwitcher::SwitcherItem::focusInEvent(QFocusEvent * e)
{
//    qDebug() << "SwitcherItem::focusInEvent" << e << objectName();
    emit infoChanged(text());
    QToolButton::focusInEvent(e);
}

void RazorAppSwitcher::SwitcherItem::mouseMoveEvent(QMouseEvent * e)
{
    if (! hasFocus())
        setFocus(Qt::MouseFocusReason);
}

void RazorAppSwitcher::SwitcherItem::mouseReleaseEvent(QMouseEvent * e)
{
    emit activateXWindow();
}
