/* coded by Ketmar // Vampire Avalon (psyc://ketmar.no-ip.org/~Ketmar)
 * (c)DWTFYW
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details.
 */
#include "cfgfile.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>

QMultiMap<QString, QString> loadCfgFile(const QString &fname, bool forceLoCase)
{
    QMultiMap<QString, QString> res;
    QFile fl(fname);
    if (fl.open(QIODevice::ReadOnly))
    {
        QTextStream stream;
        stream.setDevice(&fl);
        stream.setCodec("UTF-8");
        QString curPath = "/";
        while (1)
        {
            QString s = stream.readLine();
            if (s.isNull()) break;
            s = s.trimmed();
            if (s.isEmpty() || s[0] == '#' || s[0] == ';') continue;
            if (s[0] == '[')
            {
                // new path
                int len = s.length()-1;
                if (s[len] == ']') len--;
                s = s.mid(1, len).simplified();
                s += '/';
                curPath = s;
                continue;
            }
            int eqp = s.indexOf('=');
            if (eqp < 0) continue; // invalid entry
            QString name = s.left(eqp).simplified();
            QString value = s.mid(eqp+1).simplified();
            if (name.isEmpty()) continue; // invalid entry
            name.prepend(curPath);
            if (forceLoCase) name = name.toLower();
            res.insert(name, value);
        }
        fl.close();
    }
    return res;
}

#define XD_CODEC  "koi8-r"
void fixXDefaults(const QString &themeName)
{
    QStringList lst;
    {
        QFile fl(QDir::home().path()+"/.Xdefaults");
        if (fl.open(QIODevice::ReadOnly))
        {
            QTextStream stream;
            stream.setDevice(&fl);
            stream.setCodec(XD_CODEC);
            while (1)
            {
                QString s = stream.readLine();
                if (s.isNull()) break;
                if (!s.startsWith("Xcursor*theme:")) lst << s;
            }
            fl.close();
        }
    }
    while (lst.size() > 0)
    {
        QString s(lst[lst.size()-1]);
        if (!s.trimmed().isEmpty()) break;
        lst.removeAt(lst.size()-1);
    }
    {
        //QByteArray ba(themeName.toUtf8());
        QFile fl(QDir::home().path()+"/.Xdefaults");
        if (fl.open(QIODevice::WriteOnly))
        {
            QTextStream stream;
            stream.setDevice(&fl);
            stream.setCodec(XD_CODEC);
            foreach (const QString &s, lst)
            {
                stream << s << "\n";
            }
            stream << "\nXcursor*theme: " << themeName << "\n";
            fl.close();
        }
    }
}

const QString findDefaultTheme()
{
    QString res = "default";
    QFile fl(QDir::home().path()+"/.Xdefaults");
    if (fl.open(QIODevice::ReadOnly))
    {
        QTextStream stream;
        stream.setDevice(&fl);
        stream.setCodec(XD_CODEC);
        while (1)
        {
            QString s = stream.readLine();
            if (s.isNull()) break;
            if (!s.startsWith("Xcursor*theme:")) continue;
            s.remove(0, 14);
            s = s.trimmed();
            if (s.isEmpty()) s = "default";
            res = s;
        }
        fl.close();
    }
    return res;
}
