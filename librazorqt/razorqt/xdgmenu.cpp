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

#include "xdgmenu.h"
#include "xdgmenureader.h"
#include "domhelper.h"
#include "xdgmenurules.h"
#include "xdgmenuapplinkprocessor.h"
#include "xdgenv.h"

#include <QDebug>
#include <QtXml/QDomElement>
#include <QtXml/QDomNamedNodeMap>
#include <QtCore/QFile>
#include <QtCore/QSettings>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QHash>
#include <QtCore/QLocale>


/************************************************

 ************************************************/
XdgMenu::XdgMenu(const QString& menuFileName, QObject *parent) :
    QObject(parent)
{
    mMenuFileName = menuFileName;
    mLogDir = "";
}


/************************************************

 ************************************************/
XdgMenu::~XdgMenu()
{
}

/************************************************

 ************************************************/
void XdgMenu::setLogDir(const QString& directory)
{
    mLogDir = directory;
}


/************************************************

 ************************************************/
bool XdgMenu::read()
{

    XdgMenuReader reader;
    if (!reader.load(mMenuFileName))
    {
        qWarning() << reader.errorString();
        mErrorString = reader.errorString();
        return false;
    }

    mXml = *(reader.xml());
    QDomElement root = mXml.documentElement();
    if (!mLogDir.isEmpty())
      save(mLogDir + "/00-reader.xml");

    simplify(root);
    if (!mLogDir.isEmpty())
      save(mLogDir + "/01-simplify.xml");

    mergeMenus(root);
    if (!mLogDir.isEmpty())
      save(mLogDir + "/02-mergeMenus.xml");

    moveMenus(root);
    if (!mLogDir.isEmpty())
          save(mLogDir + "/03-moveMenus.xml");

    mergeMenus(root);
    if (!mLogDir.isEmpty())
          save(mLogDir + "/04-mergeMenus.xml");

    deleteDeletedMenus(root);
    if (!mLogDir.isEmpty())
          save(mLogDir + "/05-deleteDeletedMenus.xml");

    processDirectoryEntries(root, QStringList());
    if (!mLogDir.isEmpty())
          save(mLogDir + "/06-processDirectoryEntries.xml");

    processApps(root);
    if (!mLogDir.isEmpty())
          save(mLogDir + "/07-processApps.xml");

    deleteEmpty(root);
    if (!mLogDir.isEmpty())
          save(mLogDir + "/08-deleteEmpty.xml");

    return true;
}


/************************************************

 ************************************************/
void XdgMenu::save(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        qWarning() << QString("Cannot write file %1:\n%2.")
                .arg(fileName)
                .arg(file.errorString());
        return;
    }

    QTextStream ts(&file);
    mXml.save(ts, 2);

    file.close();
}

/************************************************

 ************************************************/
void XdgMenu::mergeMenus(QDomElement& element)
{
    QHash<QString, QDomElement> menus;

    MutableDomElementIterator it(element, "Menu");

    it.toFront();
    while(it.hasNext())
    {
        it.next();
        menus[it.current().attribute("name")] = it.current();
    }


    it.toBack();
    while (it.hasPrevious())
    {
        QDomElement src = it.previous();
        QDomElement dest = menus[src.attribute("name")];
        if (dest != src)
        {
            prependChilds(src, dest);
            element.removeChild(src);
        }
    }


    QDomElement n = element.firstChildElement("Menu");
    while (!n.isNull())
    {
        mergeMenus(n);
        n = n.nextSiblingElement("Menu");
    }

    it.toFront();
    while(it.hasNext())
        mergeMenus(it.next());
}


/************************************************

 ************************************************/
void XdgMenu::simplify(QDomElement& element)
{
    MutableDomElementIterator it(element);
    //it.toFront();
    while(it.hasNext())
    {
        QDomElement n = it.next();

        if (n.tagName() == "Name")
        {
            // The <Name> field must not contain the slash character ("/");
            // implementations should discard any name containing a slash.
            element.setAttribute("name", n.text().remove('/'));
            n.parentNode().removeChild(n);
        }

        // ......................................
        else if(n.tagName() == "Deleted")
        {
            element.setAttribute("deleted", true);
            n.parentNode().removeChild(n);
        }
        else if(n.tagName() == "NotDeleted")
        {
            element.setAttribute("deleted", false);
            n.parentNode().removeChild(n);
        }

        // ......................................
        else if(n.tagName() == "OnlyUnallocated")
        {
            element.setAttribute("onlyUnallocated", true);
            n.parentNode().removeChild(n);
        }
        else if(n.tagName() == "NotOnlyUnallocated")
        {
            element.setAttribute("onlyUnallocated", false);
            n.parentNode().removeChild(n);
        }

        // ......................................
        else if(n.tagName() == "Menu")
        {
            simplify(n);
        }

    }

}


/************************************************

 ************************************************/
void XdgMenu::prependChilds(QDomElement& srcElement, QDomElement& destElement)
{
    MutableDomElementIterator it(srcElement);

    it.toBack();
    while(it.hasPrevious())
    {
        QDomElement n = it.previous();
        destElement.insertBefore(n, destElement.firstChild());
    }

    if (srcElement.attributes().contains("deleted") &&
        !destElement.attributes().contains("deleted")
       )
        destElement.setAttribute("deleted", srcElement.attribute("deleted"));

    if (srcElement.attributes().contains("onlyUnallocated") &&
        !destElement.attributes().contains("onlyUnallocated")
       )
        destElement.setAttribute("onlyUnallocated", srcElement.attribute("onlyUnallocated"));
}


/************************************************

 ************************************************/
void XdgMenu::appendChilds(QDomElement& srcElement, QDomElement& destElement)
{
    MutableDomElementIterator it(srcElement);

    while(it.hasNext())
        destElement.appendChild(it.next());

    if (srcElement.attributes().contains("deleted"))
        destElement.setAttribute("deleted", srcElement.attribute("deleted"));

    if (srcElement.attributes().contains("onlyUnallocated"))
        destElement.setAttribute("onlyUnallocated", srcElement.attribute("onlyUnallocated"));
}


/************************************************
 Search item by path. The path can be absolute or relative. If the element not
 found, the behavior depends on a parameter "createNonExisting." If it's true, then
 the missing items will be created, otherwise the function returns 0.
 ************************************************/
QDomElement XdgMenu::findMenu(QDomElement& baseElement, const QString& path, bool createNonExisting)
{
    // Absolute path ..................
    if (path.startsWith('/'))
    {
        QDomElement root = mXml.documentElement();
        return findMenu(root, path.section('/', 2), createNonExisting);
    }

    // Relative path ..................
    if (path.isEmpty())
        return baseElement;


    QString name = path.section('/', 0, 0);
    MutableDomElementIterator it(baseElement);
    while(it.hasNext())
    {
        QDomElement n = it.next();
        if (n.attribute("name") == name)
            return findMenu(n, path.section('/', 1), createNonExisting);
    }



    // Not found ......................
    if (!createNonExisting)
        return QDomElement();


    QStringList names = path.split('/', QString::SkipEmptyParts);
    QDomElement el = baseElement;
    foreach (QString name, names)
    {
        QDomElement p = el;
        el = mXml.createElement("Menu");
        p.appendChild(el);
        el.setAttribute("name", name);
    }
    return el;

}


/************************************************
 Recurse to resolve <Move> elements for each menu starting with any child menu before
 handling the more top level menus. So the deepest menus have their <Move> operations
 performed first. Within each <Menu>, execute <Move> operations in the order that
 they appear.
 If the destination path does not exist, simply relocate the origin <Menu> element,
 and change its <Name> field to match the destination path.
 If the origin path does not exist, do nothing.
 If both paths exist, take the origin <Menu> element, delete its <Name> element, and
 prepend its remaining child elements to the destination <Menu> element.
 ************************************************/
void XdgMenu::moveMenus(QDomElement& element)
{
    {
        MutableDomElementIterator i(element, "Menu");
        while(i.hasNext())
            moveMenus(i.next());
    }

    MutableDomElementIterator i(element, "Move");
    while(i.hasNext())
    {
        i.next();
        QString oldPath = i.current().lastChildElement("Old").text();
        QString newPath = i.current().lastChildElement("New").text();

        element.removeChild(i.current());

        if (oldPath.isEmpty() || newPath.isEmpty())
            continue;

        QDomElement oldMenu = findMenu(element, oldPath, false);
        if (oldMenu.isNull())
            continue;

        QDomElement newMenu = findMenu(element, newPath, true);
        appendChilds(oldMenu, newMenu);
        oldMenu.parentNode().removeChild(oldMenu);
    }

}


/************************************************
 For each <Menu> containing a <Deleted> element which is not followed by a
 <NotDeleted> element, remove that menu and all its child menus.
 ************************************************/
void XdgMenu::deleteDeletedMenus(QDomElement& element)
{
    MutableDomElementIterator i(element, "Menu");
    while(i.hasNext())
    {
        QDomElement e = i.next();
        if (e.attribute("deleted") == "1")
            element.removeChild(e);
        else
            deleteDeletedMenus(e);
    }

}


/************************************************

 ************************************************/
void XdgMenu::processDirectoryEntries(QDomElement& element, const QStringList& parentDirs)
{
    QStringList dirs;
    QStringList files;

    MutableDomElementIterator i(element, "");
    i.toBack();
    while(i.hasPrevious())
    {
        QDomElement e = i.previous();

        if (e.tagName() == "Directory")
        {
            files << e.text();
            element.removeChild(e);
        }

        else if (e.tagName() == "DirectoryDir")
        {
            dirs << e.text();
            element.removeChild(e);
        }
    }

    dirs << parentDirs;

    bool found = false;
    foreach(QString file, files){
        if (file.startsWith('/'))
            found = loadDirectoryFile(file, element);
        else
        {
            foreach (QString dir, dirs)
            {
                found = loadDirectoryFile(dir + "/" + file, element);
                if (found) break;
            }
        }
        if (found) break;
    }


    MutableDomElementIterator it(element, "Menu");
    while(it.hasNext())
    {
        QDomElement e = it.next();
        processDirectoryEntries(e, dirs);
    }

}


/************************************************

 ************************************************/
bool XdgMenu::loadDirectoryFile(const QString& fileName, QDomElement& element)
{
    XdgDesktopFile file(fileName);

    if (!file.isValid())
        return false;

    element.setAttribute("title", file.localizedValue("Name").toString());
    element.setAttribute("comment", file.localizedValue("Comment").toString());
    element.setAttribute("icon", file.value("Icon").toString());

    return true;
}


/************************************************

 ************************************************/
void XdgMenu::processApps(QDomElement& element)
{
    XdgMenuApplinkProcessor processor(element, this);
    processor.run();
}


/************************************************

 ************************************************/
void XdgMenu::deleteEmpty(QDomElement& element)
{
    MutableDomElementIterator it(element, "Menu");
    while(it.hasNext())
        deleteEmpty(it.next());

    QDomElement childMenu = element.firstChildElement("Menu");
    QDomElement childApps = element.firstChildElement("AppLink");

    if (childMenu.isNull() && childApps.isNull())
    {
        element.parentNode().removeChild(element);
    }
}


/************************************************
 $XDG_CONFIG_DIRS/menus/${XDG_MENU_PREFIX}applications.menu
 The first file found in the search path should be used; other files are ignored.
 ************************************************/
QString XdgMenu::getMenuFileName(const QString& baseName)
{
    QStringList configDirs = XdgEnv::configDirs().split(":", QString::SkipEmptyParts);
    QString menuPrefix = getenv("XDG_MENU_PREFIX");

    foreach(QString configDir, configDirs)
    {
        QFileInfo file(QString("%1/menus/%2%3").arg(configDir, menuPrefix, baseName));
        if (file.exists())
            return file.filePath();
    }

    return "";
}
