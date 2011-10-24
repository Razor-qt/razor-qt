/* BEGIN_COMMON_COPYRIGHT_HEADER
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


#include "quicklaunchaction.h"

#include <QtDebug>
#include <QtCore/QProcess>
#include <QtCore/QUrl>
#include <QtGui/QMessageBox>
#include <QtGui/QDesktopServices>
#include <QtGui/QFileIconProvider>

#include <qtxdg/xdgdesktopfile.h>
#include <qtxdg/xdgicon.h>
#include <qtxdg/xdgmime.h>


QuickLaunchAction::QuickLaunchAction(const QString & name,
                                     const QString & exec,
                                     const QString & icon,
                                     QWidget * parent)
    : QAction(name, parent),
      m_valid(true)
{
    m_type = ActionLegacy;
    
    m_settingsMap["name"] = name;
    m_settingsMap["exec"] = exec;
    m_settingsMap["icon"] = icon;

    if (icon.isNull())
        setIcon(XdgIcon::defaultApplicationIcon());
    else
        setIcon(QIcon(icon));

    setData(exec);
    connect(this, SIGNAL(triggered()), this, SLOT(execAction()));
}

QuickLaunchAction::QuickLaunchAction(const XdgDesktopFile * xdg,
                                     QWidget * parent)
    : QAction(parent),
      m_valid(true)
{
    m_type = ActionXdg;
    
    m_settingsMap["desktop"] = xdg->fileName();

    QString title(xdg->localizedValue("Name").toString());
    QString gn(xdg->localizedValue("GenericName").toString());
    if (!gn.isEmpty())
        title += " (" + gn + ")";
    setText(title);

    setIcon(xdg->icon(XdgIcon::defaultApplicationIcon()));

    setData(xdg->fileName());
    connect(this, SIGNAL(triggered()), this, SLOT(execAction()));
}

QuickLaunchAction::QuickLaunchAction(const QString & fileName, QWidget * parent)
    : QAction(parent),
      m_valid(true)
{
    m_type = ActionFile;
    setText(fileName);
    setData(fileName);
    
    m_settingsMap["file"] = fileName;

    QFileInfo fi(fileName);
    if (fi.isDir())
    {
        QFileIconProvider ip;
        setIcon(ip.icon(fi));
    }
    else
    {
        XdgMimeInfo mi(fi);
        setIcon(mi.icon());
    }
    
    connect(this, SIGNAL(triggered()), this, SLOT(execAction()));
}

void QuickLaunchAction::execAction()
{
    QString exec(data().toString());
    qDebug() << "execAction" << exec;
    switch (m_type)
    {
        case ActionLegacy:
            QProcess::startDetached(exec);
            break;
        case ActionXdg:
        {
            XdgDesktopFile * xdg = XdgDesktopFileCache::getFile(exec);
            if (xdg->isValid())
                xdg->startDetached();
            break;
        }
        case ActionFile:
            QDesktopServices::openUrl(QUrl(exec));
            break;
    }
}
