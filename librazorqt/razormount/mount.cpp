/* BEGIN_COMMON_COPYRIGHT_HEADER
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * END_COMMON_COPYRIGHT_HEADER */
/* Based on a "MountTray" project - modified for Razor needs
    http://hatred.homelinux.net

    @date   2010-11-11
    @brief  Main application class: integrate all components

    Copyright (C) 2010 by hatred <hatred@inbox.ru>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the version 2 of GNU General Public License as
    published by the Free Software Foundation.

    For more information see LICENSE and LICENSE.ru files
*/

#include <iostream>

#include <QProcess>

#include "mount.h"

static bool mountClassic(const QString &device, QString &mount_point, QString &status, const QString &fs, const QString &options)
{
    Q_UNUSED(device);
    Q_UNUSED(mount_point);
    Q_UNUSED(status);
    Q_UNUSED(fs);
    Q_UNUSED(options);

    // TODO
    return false;
}

static bool unmountClassic(const QString &device, QString &status)
{
    Q_UNUSED(device);
    Q_UNUSED(status);
    // TODO
    return false;
}

static bool mountUdisks(const QString &device, QString &mount_point, QString &status, const QString &fs, const QString &options)
{
    QProcess    mount;
    QString     command = "udisks";
    QStringList args;

    mount.setProcessChannelMode(QProcess::MergedChannels);

    args << "--mount";

    if (!fs.isEmpty())
    {
        args << "--mount-fstype" << fs;
    }

    if (!options.isEmpty())
    {
        args << "--mount-options" << options;
    }

    args << device;

    mount.start(command, args);
    if (!mount.waitForStarted())
    {
        status = "Trouble with mount: start issue";
        return false;
    }

    if (!mount.waitForFinished())
    {
        status = "Trouble with mount: finish issue";
        return false;
    }

    //int code       = mount.exitCode();
    bool is_ok     = false;
    QString buffer = mount.readAll();


    // Stupid 'udisks' in any cases return 0 exit status!
    if (buffer.contains(QRegExp("^Mount failed:", Qt::CaseInsensitive)) == false)
    {
        is_ok = true;
    }

    if (is_ok == true)
    {
        QStringList list = buffer.trimmed().split(" ", QString::SkipEmptyParts);
        if (list.count() == 4)
        {
            mount_point = list.at(3);
        }
        status = "Ok";
    }
    else
    {
        status = buffer;
    }


    return is_ok;
}

static bool unmountUdisks(const QString &device, QString &status)
{
    QProcess    unmount;
    QString     command = "udisks";
    QStringList args;

    unmount.setProcessChannelMode(QProcess::MergedChannels);

    args << "--unmount" << device;

    unmount.start(command, args);
    if (!unmount.waitForStarted())
    {
        status = "Trouble with unmount: start issue";
        return false;
    }

    if (!unmount.waitForFinished())
    {
        status = "Trouble with unmount: finish issue";
        return false;
    }

    //int code       = unmount.exitCode();
    bool is_ok     = false;
    QString buffer = unmount.readAll();


    // Stupid 'udisks' in any cases return 0 exit status!
    if (buffer.contains(QRegExp("^Unmount failed:", Qt::CaseInsensitive)) == false)
    {
        is_ok = true;
    }

    if (is_ok == true)
    {
        status = "Ok";
    }
    else
    {
        status = buffer;
    }

    return is_ok;
}


////////////////////////////////////////////////////////////////////////////////////////////////////


bool diskMount(MountingType  type,
               const QString      &device,
               QString            &mount_point,
               QString            &status,
               const QString      &fs,
               const QString      &options )
{
    bool result = false;
    switch (type)
    {
        case CLASSIC:
        {
            result = mountClassic(device, mount_point, status, fs, options);
            break;
        }

        case UDISKS:
        {
            result = mountUdisks(device, mount_point, status, fs, options);
            break;
        }
    }

    return result;
}

bool diskUnMount(MountingType type, const QString &device, QString &status)
{
    bool result = false;
    switch (type)
    {
        case CLASSIC:
        {
            result = unmountClassic(device, status);
            break;
        }

        case UDISKS:
        {
            result = unmountUdisks(device, status);
        }
    }

    return result;
}

QStringList isMounted(const QString &name, MountCheck check)
{
    QStringList return_value;

    QFile file("/etc/mtab");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "isMounted: cannot open /etc/mtab";
        return return_value;
    }

    QTextStream in(&file);
    // /etc/mtab is symlink to /proc... here
    //
    // On Unix, there are some special system files (e.g. in /proc) for which size()
    // will always return 0, yet you may still be able to read more data from such
    // a file; the data is generated in direct response to you calling read(). In
    // this case, however, you cannot use atEnd()
    QString line = in.readLine();
    QString value;
    while(!line.isNull())
    {
        QStringList parts = line.split(QRegExp("\\s+"));

        switch(check)
        {
            case MC_DEVICE:
                value = parts[0];
                break;

            case MC_DIR:
                value = parts[1];
                break;
        }

        if (value == name)
        {
            return_value = parts;
            break;
        }
        
        line = in.readLine();
    }

    return return_value;
}
