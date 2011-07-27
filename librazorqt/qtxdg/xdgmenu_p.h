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

#include "xdgmenu.h"
#include <QtCore/QObject>
#include <QtCore/QFileSystemWatcher>

class QDomElement;
class QStringList;
class QString;
class QDomDocument;

class XdgMenuPrivate: QObject
{
Q_OBJECT
public:
    XdgMenuPrivate(XdgMenu* parent);

    void simplify(QDomElement& element);
    void mergeMenus(QDomElement& element);
    void moveMenus(QDomElement& element);
    void deleteDeletedMenus(QDomElement& element);
    void processDirectoryEntries(QDomElement& element, const QStringList& parentDirs);
    void processApps(QDomElement& element);
    void deleteEmpty(QDomElement& element);
    void processLayouts(QDomElement& element);
    void fixSeparators(QDomElement& element);

    bool loadDirectoryFile(const QString& fileName, QDomElement& element);
    void prependChilds(QDomElement& srcElement, QDomElement& destElement);
    void appendChilds(QDomElement& srcElement, QDomElement& destElement);

    void saveLog(const QString& logFileName);

    void clearWatcher();

    QString mErrorString;
    QStringList mEnvironments;
    QString mMenuFileName;
    QString mLogDir;
    QDomDocument mXml;

    QFileSystemWatcher mWatcher;
    bool mOutDated;
public slots:
   void fileChanged(const QString& path);

private:
    XdgMenu* const q_ptr;
    Q_DECLARE_PUBLIC(XdgMenu)
};

