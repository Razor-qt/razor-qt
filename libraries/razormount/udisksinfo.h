/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */
 
#ifndef UDISKSINFO_H
#define UDISKSINFO_H

#include <QtCore/QObject>
#include <QtDBus>

class UdisksInfo : public QObject
{

    Q_OBJECT

public :

    enum MediaType {
        Unknown,
        USB,
        Optical
    };

    UdisksInfo(QDBusObjectPath device, QObject *parent=0);
    
    bool isExternal() { return m_isExternal; }
    bool isMounted() { return m_isMounted; }
    QString udisksPath() { return m_udisksPath; }
    QString displayName() { return m_displayName; }
    QString path() { return m_path; }
    QString fileSystem() { return m_fileSystem; }
    
    QString iconName() const;
    
    bool mount();
    bool unmount();
    bool eject();

signals :
    void error(const QString &msg);
    void success();

private slots :
    void m_dbus_success(const QDBusMessage &msg);
    void m_dbus_error(const QDBusError &err, const QDBusMessage &msg);

private :
    QString m_path;
    QString m_fileSystem;
    qulonglong m_size;

    MediaType m_type;

    QString m_udisksPath;
    QString m_displayName;

    bool m_isExternal;
    bool m_isMounted;
    bool m_isEjectable;

    QDBusInterface *m_dbus;
};

#endif
