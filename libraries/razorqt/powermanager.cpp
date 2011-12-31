/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL3+
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

#include "powermanager.h"
#include "razorpower/razorpower.h"
#include <qtxdg/xdgicon.h>
#include <QDBusInterface>
#include <QMessageBox>
#include <QApplication>
#include <QDesktopWidget>
#include <QtDebug>
#include "libtranslate.h"

class MessageBox: public QMessageBox
{
public:
    explicit MessageBox(QWidget *parent = 0): QMessageBox(parent) {}

    static QMessageBox::StandardButton question(QWidget* parent,
                                         const QString& title,
                                         const QString& text,
                                         StandardButton button0,
                                         StandardButton button1)
    {
        MessageBox msgBox(parent);
        msgBox.setWindowTitle(title);
        msgBox.setText(text);
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

        if (msgBox.exec() == QMessageBox::Yes)
            return QMessageBox::Yes;
        else
            return QMessageBox::No;
    }


protected:
    virtual void resizeEvent(QResizeEvent* event)
    {
        QRect screen = QApplication::desktop()->screenGeometry();
        move((screen.width()  - this->width()) / 2,
             (screen.height() - this->height()) / 2);

    }
};

PowerManager::PowerManager(QObject * parent)
    : QObject(parent),
    m_parentWidget(0)
{
    libTranslate("librazorqt");
    m_power = new RazorPower(this);
//    connect(m_power, SIGNAL(suspendFail()), this, SLOT(suspendFailed()));
//    connect(m_power, SIGNAL(hibernateFail()), this, SLOT(hibernateFailed()));
//    connect(m_power, SIGNAL(monitoring(const QString &)),
//            this, SLOT(monitoring(const QString&)));
}

PowerManager::~PowerManager()
{
//    delete m_power;
}

QList<QAction*> PowerManager::availableActions()
{
    QList<QAction*> ret;
    QAction * act;

    // TODO/FIXME: icons
    if (m_power->canHibernate())
    {
        act = new QAction(XdgIcon::fromTheme("system-suspend-hibernate"), tr("Hibernate"), this);
        connect(act, SIGNAL(triggered()), this, SLOT(hibernate()));
        ret.append(act);
    }

    if (m_power->canSuspend())
    {
        act = new QAction(XdgIcon::fromTheme("system-suspend"), tr("Suspend"), this);
        connect(act, SIGNAL(triggered()), this, SLOT(suspend()));
        ret.append(act);
    }

    if (m_power->canReboot())
    {
        act = new QAction(XdgIcon::fromTheme("system-reboot"), tr("Reboot"), this);
        connect(act, SIGNAL(triggered()), this, SLOT(reboot()));
        ret.append(act);
    }

    if (m_power->canShutdown())
    {
        act = new QAction(XdgIcon::fromTheme("system-shutdown"), tr("Shutdown"), this);
        connect(act, SIGNAL(triggered()), this, SLOT(shutdown()));
        ret.append(act);
    }

    if (m_power->canLogout())
    {
        act = new QAction(XdgIcon::fromTheme("system-log-out"), tr("Logout"), this);
        connect(act, SIGNAL(triggered()), this, SLOT(logout()));
        ret.append(act);
    }

    return ret;
}
    



void PowerManager::suspend()
{
     if (MessageBox::question(m_parentWidget, tr("Razor Session Suspend"),
                            tr("Do you want to really suspend your computer?<p>Suspends the computer into a low power state. System state is not preserved if the power is lost."),
                            QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
    {
        return;
    }

    m_power->suspend();
}

void PowerManager::hibernate()
{
    if (MessageBox::question(m_parentWidget, tr("Razor Session Hibernate"),
                             tr("Do you want to really hibernate your computer?<p>Hibernates the computer into a low power state. System state is preserved if the power is lost."),
                             QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
    {
        return;
    }

    m_power->hibernate();
}

void PowerManager::reboot()
{
    if (MessageBox::question(m_parentWidget, tr("Razor Session Reboot"),
                             tr("Do you want to really restart your computer? All unsaved work will be lost..."),
                             QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
    {
        return;
    }

    m_power->reboot();
}

void PowerManager::shutdown()
{
    if (MessageBox::question(m_parentWidget, tr("Razor Session Shutdown"),
                             tr("Do you want to really switch off your computer? All unsaved work will be lost..."),
                             QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
    {
        return;
    }

    m_power->shutdown();
}

void PowerManager::logout()
{
    if (MessageBox::question(m_parentWidget, tr("Razor Session Logout"),
                             tr("Do you want to really logout? All unsaved work will be lost..."),
                             QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
    {
        return;
    }

    m_power->logout();
}

void PowerManager::hibernateFailed()
{
    QMessageBox::warning(m_parentWidget, tr("Razor Power Manager Error"), tr("Hibernate failed."));
}

void PowerManager::suspendFailed()
{
    QMessageBox::warning(m_parentWidget, tr("Razor Power Manager Error"), tr("Suspend failed."));
}
