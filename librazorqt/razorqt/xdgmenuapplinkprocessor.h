/********************************************************************
  Copyright: 2010 Alexander Sokoloff <sokoloff.a@gmail.ru>

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

#ifndef XDGMENUAPPLINKPROCESSOR_H
#define XDGMENUAPPLINKPROCESSOR_H

#include "xdgmenurules.h"
#include <QObject>
#include <QDomElement>
#include <QLinkedList>
#include <QString>
#include <QHash>

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
    explicit XdgMenuAppFileInfo(XdgDesktopFile* desktopFile, QObject *parent)
        : QObject(parent)
    {
        mDesktopFile = desktopFile;
        mAllocated = false;
    }

    XdgDesktopFile* desktopFile() const { return mDesktopFile; }
    bool allocated() const { return mAllocated; }
    void setAllocated(bool value) { mAllocated = value; }

private:
    XdgDesktopFile* mDesktopFile;
    bool mAllocated;
};


#endif // XDGMENUAPPLINKPROCESSOR_H
