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


#include "providers.h"
#include <qtxdg/xdgicon.h>
#include <qtxdg/xdgdesktopfile.h>
#include <qtxdg/xdgmenu.h>
#include <qtxdg/xmlhelper.h>
#include <qtxdg/xdgdirs.h>

#include <QtCore/QProcess>
#include <QtCore/QtAlgorithms>
#include <QtCore/QFileInfo>
#include <QtCore/QSettings>

#define MAX_HISORTY 100


/************************************************

 ************************************************/
CommandProviderItem::CommandProviderItem()
{
}


/************************************************

 ************************************************/
CommandProviderItem::~CommandProviderItem()
{
}


/************************************************

 ************************************************/
void CommandProviderItem::setTile(const QString &title)
{
    mTitle = title;
}


/************************************************

 ************************************************/
void CommandProviderItem::setComment(const QString &comment)
{
    mComment = comment;
}




/************************************************

 ************************************************/
CommandProvider::CommandProvider():
    QList<CommandProviderItem*>()
{
}


/************************************************

 ************************************************/
CommandProvider::~CommandProvider()
{
    qDeleteAll(*this);
}




/************************************************

 ************************************************/
AppLinkItem::AppLinkItem(const QDomElement &element):
    CommandProviderItem()
{
    setIcon(XdgIcon::fromTheme(element.attribute("icon")));
    setTile(element.attribute("title"));
    setComment(element.attribute("genericName"));
    setToolTip(element.attribute("comment"));

    QString command = QFileInfo(element.attribute("exec")).baseName().section(" ", 0, 0);
    mSearchText = element.attribute("title") + " " + command;
    mDesktopFile = element.attribute("desktopFile");

}


/************************************************

 ************************************************/
bool AppLinkItem::run() const
{
    XdgDesktopFile *desktop = XdgDesktopFileCache::getFile(mDesktopFile);
    return desktop->startDetached();
}


/************************************************

 ************************************************/
bool AppLinkItem::compare(const QRegExp &regExp) const
{
    if (regExp.isEmpty())
        return false;

    QRegExp re(regExp);

    re.setCaseSensitivity(Qt::CaseInsensitive);
    return mSearchText.contains(re);
}




/************************************************

 ************************************************/
AppLinkProvider::AppLinkProvider():
    CommandProvider(),
    mXdgMenu( new XdgMenu())
{
    mXdgMenu->setEnvironments("X-RAZOR");
}


/************************************************

 ************************************************/
AppLinkProvider::~AppLinkProvider()
{
    delete mXdgMenu;
}


/************************************************

 ************************************************/
bool AppLinkProvider::isOutDated() const
{
    return mXdgMenu->isOutDated();
}


/************************************************

 ************************************************/
void AppLinkProvider::rebuild()
{
    mXdgMenu->read(XdgMenu::getMenuFileName());

    qDeleteAll(*this);
    clear();
    rebuildMainMenu(mXdgMenu->xml().documentElement());
}


/************************************************

 ************************************************/
void AppLinkProvider::rebuildMainMenu(const QDomElement &xml)
{
    DomElementIterator it(xml, "");
    while(it.hasNext())
    {
        QDomElement e = it.next();

        // Build submenu ........................
        if (e.tagName() == "Menu")
            rebuildMainMenu(e);

        //Build application link ................
        else if (e.tagName() == "AppLink")
        {
            AppLinkItem *item = new AppLinkItem(e);
            append(item);
        }
    }
}




/************************************************

 ************************************************/
HistoryItem::HistoryItem(const QString &command):
    CommandProviderItem()
{
    setIcon(XdgIcon::defaultApplicationIcon());
    setTile(command);
    setComment(QObject::tr("History"));
    mCommand = command;
}


/************************************************

 ************************************************/
bool HistoryItem::run() const
{
    return QProcess::startDetached(mCommand);
}


/************************************************

 ************************************************/
bool HistoryItem::compare(const QRegExp &regExp) const
{
    QRegExp re(regExp);

    re.setCaseSensitivity(Qt::CaseSensitive);
    return mCommand.contains(re);
}





/************************************************

 ************************************************/
HistoryProvider::HistoryProvider():
    CommandProvider()
{
    QString fileName = (XdgDirs::cacheHome() + "/razor-runner.history");
    mHistoryFile = new QSettings(fileName, QSettings::IniFormat);
    mHistoryFile->beginGroup("commands");
    for (uint i=0; i<MAX_HISORTY; ++i)
    {
        QString key = QString("%1").arg(i, 3, 10, QChar('0'));
        if (mHistoryFile->contains(key))
        {
            HistoryItem *item = new HistoryItem(mHistoryFile->value(key).toString());
            append(item);
        }
    }
}


/************************************************

 ************************************************/
HistoryProvider::~HistoryProvider()
{
    delete mHistoryFile;
}


/************************************************

 ************************************************/
void HistoryProvider::AddCommand(const QString &command)
{
    HistoryItem *item = new HistoryItem(command);
    insert(0, item);

    mHistoryFile->clear();
    for (int i=0; i<qMin(length(), MAX_HISORTY); ++i)
    {
        QString key = QString("%1").arg(i, 3, 10, QChar('0'));
        mHistoryFile->setValue(key, static_cast<HistoryItem*>(at(i))->command());
    }
}


#ifdef MATH_ENABLED
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>

/************************************************

 ************************************************/
MathItem::MathItem():
    CommandProviderItem()
{
    setToolTip(QObject::tr("Mathematics"));
    setIcon(XdgIcon::fromTheme("accessories-calculator"));
}


/************************************************

 ************************************************/
bool MathItem::run() const
{
    return false;
}


/************************************************

 ************************************************/
bool MathItem::compare(const QRegExp &regExp) const
{
    QString s = regExp.pattern().trimmed();

    if (s.endsWith("="))
    {
        s.chop(1);
        QScriptEngine myEngine;
        QScriptValue res = myEngine.evaluate(s);
        if (res.isNumber())
        {
            MathItem *self=const_cast<MathItem*>(this);
            self->setTile(s + " = " + res.toString());
            return true;
        }
    }

    return false;
}



/************************************************

 ************************************************/
MathProvider::MathProvider()
{
    append(new MathItem());
}

#endif
