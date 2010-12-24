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

#ifndef XDGMENUREADER_H
#define XDGMENUREADER_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

class XdgMenuReader : public QObject
{
    Q_OBJECT
public:
    explicit XdgMenuReader(XdgMenuReader*  parentReader = 0, QObject *parent = 0);
    virtual ~XdgMenuReader();

    bool load(const QString& fileName, const QString& baseDir = "");
    QString fileName() const { return mFileName; }
    QString errorString() const { return mErrorStr; }
    QDomDocument* xml() { return &mXml; }

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
};

#endif // XDGMENUREADER_H
