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


#ifndef QTXDG_XDGMENUREADER_H
#define QTXDG_XDGMENUREADER_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
class XdgMenu;
class XdgMenuReader : public QObject
{
    Q_OBJECT
public:
    explicit XdgMenuReader(XdgMenu* menu, XdgMenuReader*  parentReader = 0, QObject *parent = 0);
    virtual ~XdgMenuReader();

    bool load(const QString& fileName, const QString& baseDir = "");
    QString fileName() const { return mFileName; }
    QString errorString() const { return mErrorStr; }
    QDomDocument& xml() { return mXml; }

signals:

public slots:

protected:
    void processMergeTags(QDomElement& element);
    void processMergeFileTag(QDomElement& element, QStringList* mergedFiles);
    void processMergeDirTag(QDomElement& element, QStringList* mergedFiles);
    void processDefaultMergeDirsTag(QDomElement& element, QStringList* mergedFiles);

    void processAppDirTag(QDomElement& element);
    void processDefaultAppDirsTag(QDomElement& element);

    void processDirectoryDirTag(QDomElement& element);
    void processDefaultDirectoryDirsTag(QDomElement& element);
    void addDirTag(QDomElement& previousElement, const QString& tagName, const QString& dir);

    void mergeFile(const QString& fileName, QDomElement& element, QStringList* mergedFiles);
    void mergeDir(const QString& dirName, QDomElement& element, QStringList* mergedFiles);

private:
    QString mFileName;
    QString mDirName;
    QString mErrorStr;
    QDomDocument mXml;
    XdgMenuReader*  mParentReader;
    QStringList mBranchFiles;
    XdgMenu* mMenu;
};

#endif // QTXDG_XDGMENUREADER_H
