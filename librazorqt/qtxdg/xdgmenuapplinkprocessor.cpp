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


#include "xdgmenu.h"
#include "xdgmenuapplinkprocessor.h"
#include "xmlhelper.h"
#include "xdgdesktopfile.h"

#include <QDir>


/************************************************

 ************************************************/
XdgMenuApplinkProcessor::XdgMenuApplinkProcessor(QDomElement& element,  XdgMenu* menu, XdgMenuApplinkProcessor *parent) :
    QObject(parent)
{
    mElement = element;
    mParent = parent;
    mMenu = menu;

    mOnlyUnallocated = element.attribute("onlyUnallocated") == "1";

    MutableDomElementIterator i(element, "Menu");
    while(i.hasNext())
    {
        QDomElement e = i.next();
        mChilds.append(new XdgMenuApplinkProcessor(e, mMenu, this));
    }

}


/************************************************

 ************************************************/
XdgMenuApplinkProcessor::~XdgMenuApplinkProcessor()
{
}


/************************************************

 ************************************************/
void XdgMenuApplinkProcessor::run()
{
    step1();
    step2();
}


/************************************************

 ************************************************/
void XdgMenuApplinkProcessor::step1()
{
    fillAppFileInfoList();
    createRules();

    // Check Include rules & mark as allocated ............
    XdgMenuAppFileInfoHashIterator i(mAppFileInfoHash);
    while(i.hasNext())
    {
        i.next();
        XdgDesktopFile* file = i.value()->desktopFile();

        if (mRules.checkInclude(i.key(), *file))
        {
            if (!mOnlyUnallocated)
                i.value()->setAllocated(true);

            if (!mRules.checkExclude(i.key(), *file))
            {
                mSelected.append(i.value());
            }

        }
    }

    // Process childs menus ...............................
    foreach (XdgMenuApplinkProcessor* child, mChilds)
        child->step1();
}


/************************************************

 ************************************************/
void XdgMenuApplinkProcessor::step2()
{
    // Create AppLinks elements ...........................
    QDomDocument doc = mElement.ownerDocument();

    foreach (XdgMenuAppFileInfo* fileInfo, mSelected)
    {
        if (mOnlyUnallocated && fileInfo->allocated())
            continue;

        XdgDesktopFile* file = fileInfo->desktopFile();

        // Means "this application exists, but don't display it in the menus".
        if (file->value("NoDisplay").toBool())
            continue;

        // Hidden should have been called Deleted. It means the user deleted
        // (at his level) something that was present
        if (file->value("Hidden").toBool())
            continue;

        // File name of a binary on disk used to determine if the program is
        // actually installed. If not, entry may not show in menus, etc.
        QString s = file->value("TryExec").toString();
        if (!s.isEmpty() && !checkTryExec(s))
            continue;

        // A list of strings identifying the environments that should display/not
        // display a given desktop entry.
        // OnlyShowIn ........
        if (file->contains("OnlyShowIn"))
        {
            QString s = ";" + file->value("OnlyShowIn").toString() + ";";
            bool show = false;
            foreach (QString env, mMenu->environments())
            {
                if (s.contains(env))
                {
                    show = true;
                    break;
                }
            }

            if (!show)
                continue;
        }

        // NotShowIn .........
        if (file->contains("NotShowIn"))
        {
            QString s = ";" + file->value("NotShowIn").toString() + ";";
            bool show = true;
            foreach (QString env, mMenu->environments())
            {
                if (s.contains(env))
                {
                    show = false;
                    break;
                }
            }

            if (!show)
                continue;
        }



        QDomElement appLink = doc.createElement("AppLink");

        appLink.setAttribute("id", fileInfo->id());
        appLink.setAttribute("title", file->localizedValue("Name").toString());
        appLink.setAttribute("comment", file->localizedValue("Comment").toString());
        appLink.setAttribute("genericName", file->localizedValue("GenericName").toString());
        appLink.setAttribute("exec", file->value("Exec").toString());
        appLink.setAttribute("terminal", file->value("Terminal").toBool());
        appLink.setAttribute("startupNotify", file->value("StartupNotify").toBool());
        appLink.setAttribute("path", file->value("Path").toString());
        appLink.setAttribute("icon", file->value("Icon").toString());
        appLink.setAttribute("desktopFile", file->fileName());

        mElement.appendChild(appLink);

    }


    // Process childs menus ...............................
    foreach (XdgMenuApplinkProcessor* child, mChilds)
        child->step2();
}


/************************************************
 For each <Menu> element, build a pool of desktop entries by collecting entries found
 in each <AppDir> for the menu element. If two entries have the same desktop-file id,
 the entry for the earlier (closer to the top of the file) <AppDir> must be discarded.

 Next, add to the pool the entries for any <AppDir>s specified by ancestor <Menu>
 elements. If a parent menu has a duplicate entry (same desktop-file id), the entry
 for the child menu has priority.
 ************************************************/
void XdgMenuApplinkProcessor::fillAppFileInfoList()
{
    // Build a pool by collecting entries found in <AppDir>
    {
        MutableDomElementIterator i(mElement, "AppDir");
        i.toBack();
        while(i.hasPrevious())
        {
            QDomElement e = i.previous();
            findDesktopFiles(e.text(), "");
            mElement.removeChild(e);
        }
    }

    // Add the entries for ancestor <Menu> ................
    if (mParent)
    {
        XdgMenuAppFileInfoHashIterator i(mParent->mAppFileInfoHash);
        while(i.hasNext())
        {
            i.next();
            //if (!mAppFileInfoHash.contains(i.key()))
                mAppFileInfoHash.insert(i.key(), i.value());
        }
    }
}


/************************************************

 ************************************************/
void XdgMenuApplinkProcessor::findDesktopFiles(const QString& dirName, const QString& prefix)
{
    QDir dir(dirName);
    mMenu->addWatchPath(dir.absolutePath());
    QFileInfoList files = dir.entryInfoList(QStringList("*.desktop"), QDir::Files);

    foreach (QFileInfo file, files)
    {
        XdgDesktopFile* f = XdgDesktopFileCache::getFile(file.canonicalFilePath());
        if (f)
            mAppFileInfoHash.insert(prefix + file.fileName(), new XdgMenuAppFileInfo(f, prefix + file.fileName(), this));
    }


    // Working recursively ............
    QFileInfoList dirs = dir.entryInfoList(QStringList(), QDir::Dirs | QDir::NoDotAndDotDot);
    foreach (QFileInfo d, dirs)
    {
        QString dn = d.canonicalFilePath();
        if (dn != dirName)
        {
            findDesktopFiles(dn, QString("%1%2-").arg(prefix, d.fileName()));
        }
    }
}


/************************************************
 Create rules
 ************************************************/
void XdgMenuApplinkProcessor::createRules()
{
    MutableDomElementIterator i(mElement, "");
    while(i.hasNext())
    {
        QDomElement e = i.next();
        if (e.tagName()=="Include")
        {
            mRules.addInclude(e);
            mElement.removeChild(e);
        }

        else if (e.tagName()=="Exclude")
        {
            mRules.addExclude(e);
            mElement.removeChild(e);
        }
    }

}


/************************************************
 Check if the program is actually installed.
 ************************************************/
bool XdgMenuApplinkProcessor::checkTryExec(const QString& progName)
{
    if (progName.startsWith(QDir::separator()))
        return QFileInfo(progName).isExecutable();

    QStringList dirs = QString(getenv("PATH")).split(":");

    foreach (QString dir, dirs)
    {
        if (QFileInfo(QDir(dir), progName).isExecutable())
            return true;
    }

    return false;
}

