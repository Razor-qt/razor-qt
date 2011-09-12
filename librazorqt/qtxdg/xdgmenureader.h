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
