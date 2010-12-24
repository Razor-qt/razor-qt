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

#ifndef XDGMENU_H
#define XDGMENU_H

#include "xdgdesktopfile.h"

#include <QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QHash>
#include <QtXml/QDomDocument>


class QDomDocument;
class QDomElement;

class XdgMenu : public QObject
{
Q_OBJECT
friend class XdgMenuItem;

public:
    explicit XdgMenu(const QString& menuFileName, QObject *parent = 0);
    virtual ~XdgMenu();

    bool read();
    void save(const QString& fileName);

    const QDomDocument xml() const { return mXml; }
    QString menuFileName() const { return mMenuFileName; }

    QDomElement findMenu(QDomElement& baseElement, const QString& path, bool createNonExisting);
    QStringList& environments() { return mEnvironments; }

    const QString errorString() const { return mErrorString; }
signals:

public slots:

protected:
    void simplify(QDomElement& element);
    void mergeMenus(QDomElement& element);
    void moveMenus(QDomElement& element);
    void deleteDeletedMenus(QDomElement& element);

    void processDirectoryEntries(QDomElement& element, const QStringList& parentDirs);
    void processApps(QDomElement& element);
    void deleteEmpty(QDomElement& element);

    bool loadDirectoryFile(const QString& fileName, QDomElement& element);


private:
    QString mMenuFileName;
    QDomDocument mXml;
//    QHash<QString, XdgDesktopFile*> mDesktopFiles;

    void prependChilds(QDomElement& srcElement, QDomElement& destElement);
    void appendChilds(QDomElement& srcElement, QDomElement& destElement);
    QStringList mEnvironments;
    QString mErrorString;
};


#endif // XDGMENU_H
