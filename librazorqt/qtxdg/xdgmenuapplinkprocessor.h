/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * https://sourceforge.net/projects/razor-qt/
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


#ifndef QTXDG_XDGMENUAPPLINKPROCESSOR_H
#define QTXDG_XDGMENUAPPLINKPROCESSOR_H

#include "xdgmenurules.h"
#include <QtCore/QObject>
#include <QtXml/QDomElement>
#include <QtCore/QLinkedList>
#include <QtCore/QString>
#include <QtCore/QHash>

class XdgMenu;
class XdgMenuAppFileInfo;
class XdgDesktopFile;

typedef QLinkedList<XdgMenuAppFileInfo*> XdgMenuAppFileInfoList;

typedef QHash<QString, XdgMenuAppFileInfo*> XdgMenuAppFileInfoHash;
typedef QHashIterator<QString, XdgMenuAppFileInfo*> XdgMenuAppFileInfoHashIterator;

class XdgMenuApplinkProcessor : public QObject
{
    Q_OBJECT
public:
    explicit XdgMenuApplinkProcessor(QDomElement& element, XdgMenu* menu, XdgMenuApplinkProcessor *parent = 0);
    virtual ~XdgMenuApplinkProcessor();
    void run();

protected:
    void step1();
    void step2();
    void fillAppFileInfoList();
    void findDesktopFiles(const QString& dirName, const QString& prefix);

    //bool loadDirectoryFile(const QString& fileName, QDomElement& element);
    void createRules();
    bool checkTryExec(const QString& progName);

private:
    XdgMenuApplinkProcessor* mParent;
    QLinkedList<XdgMenuApplinkProcessor*> mChilds;
    XdgMenuAppFileInfoHash mAppFileInfoHash;
    XdgMenuAppFileInfoList mSelected;
    QDomElement mElement;
    bool mOnlyUnallocated;

    XdgMenu* mMenu;
    XdgMenuRules mRules;
};


class XdgMenuAppFileInfo: public QObject
{
    Q_OBJECT
public:
    explicit XdgMenuAppFileInfo(XdgDesktopFile* desktopFile, const QString& id,  QObject *parent)
        : QObject(parent)
    {
        mDesktopFile = desktopFile;
        mAllocated = false;
        mId = id;
    }

    XdgDesktopFile* desktopFile() const { return mDesktopFile; }
    bool allocated() const { return mAllocated; }
    void setAllocated(bool value) { mAllocated = value; }
    QString id() const { return mId; }
private:
    XdgDesktopFile* mDesktopFile;
    bool mAllocated;
    QString mId;
};


#endif // QTXDG_XDGMENUAPPLINKPROCESSOR_H
