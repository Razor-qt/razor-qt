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

#ifndef XDGDESKTOPFILE_H
#define XDGDESKTOPFILE_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QStringList>
#include <QIcon>

class XdgDesktopFilePrivate;

class XdgDesktopFile : protected QObject
{
    Q_OBJECT
public:
    explicit XdgDesktopFile(QObject *parent = 0, const QString& prefix="Desktop Entry");
    explicit XdgDesktopFile(const QString& fileName, QObject *parent = 0, const QString& prefix="Desktop Entry");
    XdgDesktopFile(const XdgDesktopFile& other, QObject *parent = 0);

    virtual ~XdgDesktopFile();
    XdgDesktopFile& operator=(const XdgDesktopFile& other);

    QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const;
    QVariant localizedValue(const QString& key, const QVariant& defaultValue = QVariant()) const;

    bool contains(const QString& key) const;

    bool isValid() const;
    QString fileName() const;

    bool isShow(const QString& environment = "RAZOR") const;

    QString expandExecString(const QStringList& urls = QStringList()) const;

    bool startDetached(const QStringList& urls) const;
    bool startDetached(const QString& url="") const;

    QIcon const icon(int size, const QIcon& fallback = QIcon()) const;

private:
    XdgDesktopFilePrivate* const d_ptr;
    Q_DECLARE_PRIVATE(XdgDesktopFile);
};

typedef QList<XdgDesktopFile*> XdgDesktopFileList;


class XdgDesktopFileCache
{
public:
    static XdgDesktopFile* getFile(const QString& fileName);
};


#endif // XDGDESKTOPFILE_H


