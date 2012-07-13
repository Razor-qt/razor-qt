/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Aaron Lewis <the.warl0ck.1989@gmail.com>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */
#include <QProcess>
#include <QSettings>
#include <QMap>
#include <QKeySequence>
#include <QObject>
#include <QDebug>
#include <QDBusMessage>
#include <QDBusConnection>

#include <razorqt/razorsettings.h>
#include <razorqxt/qxtglobalshortcut.h>

class AbstractShortcut : public QObject
{
    Q_OBJECT
    public:
       AbstractShortcut (QObject *parent = 0) {}

       /* Execute the command */
       virtual bool exec() = 0;
};

class CommandShortcut : public AbstractShortcut
{
    Q_OBJECT
    public:
        CommandShortcut(const QString & cmd, QObject *parent = 0);
        bool exec();

    private:
        QString m_cmd;

};

class DBusShortcut : public AbstractShortcut
{
    Q_OBJECT
    public:
        DBusShortcut( const QString & dest, 
                const QString & path,
                const QString & interface,
                const QString & method,
                const QList<QVariant> & parameters = QList<QVariant>(),
                QObject *parent = 0);
        
        bool exec();   

    private:
        QString m_dest,           /* DBUS destination */
                m_path,           /* DBUS path */
                m_interface,      /* Call interface */
                m_method;         /* Call method */

        QList<QVariant> m_params; /* Call parameters */

};

class GlobalAccel: public QObject
{
    Q_OBJECT

public:
    ~GlobalAccel();
    GlobalAccel(QObject *parent = 0);

    void bindDefault();

private:
    RazorSettings *m_shortcutSettings;
    QMap<QKeySequence, AbstractShortcut*> mapping;

private slots:
    void launchApp();
    void rebindAll();
};
