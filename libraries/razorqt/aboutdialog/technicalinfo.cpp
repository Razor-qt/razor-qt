/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
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


#include "technicalinfo.h"
#include <qtxdg/xdgdirs.h>

class TechInfoTable
{
public:
    TechInfoTable(const QString &title);
    void add(const QString &name, const QVariant &value);
    QString html() const;
    QString text(int nameFieldWidth) const;
    int maxNameLength() const;
private:
    QString mTitle;
    QList<QPair<QString,QString> > mRows;
};



TechInfoTable::TechInfoTable(const QString &title)
{
    mTitle = title;
}

void TechInfoTable::add(const QString &name, const QVariant &value)
{
    QPair<QString,QString> row;
    row.first = name;
    row.second = value.toString();
    mRows.append(row);
}

QString TechInfoTable::html() const
{
    QString res;

    res = "<style TYPE='text/css'> "
            ".techInfoKey { white-space: nowrap ; margin: 0 20px 0 16px; } "
          "</style>";

    res += QString("<b>%1</b>").arg(mTitle);
    res += "<table width='100%'>";
    QPair<QString,QString> row;
    foreach(row, mRows)
    {
        res += QString("<tr>"
                       "<td class=techInfoTd width='1%'>"
                            "<div class=techInfoKey>%1</div>"
                        "</td>"
                        "<td>%2</td>"
                       "</tr>").arg(row.first, row.second);
    }

    res += "</table>";
    return res;
}

QString TechInfoTable::text(int nameFieldWidth) const
{
    QString res;
    res += QString("%1\n").arg(mTitle);

    QPair<QString,QString> row;
    foreach(row, mRows)
    {
        res += QString("  %1  %2\n")
                .arg(row.first + ":", -nameFieldWidth)
                .arg(row.second);
    }
    return res;
}

int TechInfoTable::maxNameLength() const
{
    int res = 0;
    QPair<QString,QString> row;
    foreach (row, mRows)
        res = qMax(res, row.first.length());

    return res;
}


QString TechnicalInfo::html() const
{
    QString res;
    foreach (TechInfoTable* item, mItems)
    {
        res += item->html();
        res += "<br><br>";
    }
    return res;
}

QString TechnicalInfo::text() const
{
    int nameWidth = 0;
    foreach (TechInfoTable* item, mItems)
        nameWidth = qMax(nameWidth, item->maxNameLength());

    QString res;
    foreach (TechInfoTable* item, mItems)
    {
        res += item->text(nameWidth + 2);
        res += "\n\n";
    }
    return res;
}

TechInfoTable *TechnicalInfo::newTable(const QString &title)
{
    TechInfoTable *table = new TechInfoTable(title);
    mItems.append(table);
    return table;
}

TechnicalInfo::~TechnicalInfo()
{
    qDeleteAll(mItems);
}


TechnicalInfo::TechnicalInfo()
{
    TechInfoTable *table;

    // ******************************************
    table = newTable("Razor Desktop Toolbox - Technical Info");
#ifdef DEBUG
    QString buildType("Debug");
#else
    QString buildType("Release");
#endif

    table->add("Version",              RAZOR_VERSION);
    table->add("Qt",                   qVersion());
    table->add("Build type",           buildType);
    table->add("System Configuration", RAZOR_ETC_XDG_DIRECTORY);
    table->add("Share Directory",      SHARE_DIR);
    table->add("Translations",         TRANSLATIONS_DIR);


    // ******************************************
    table = newTable("User Directories");
    XdgDirs xdgDirs;

    table->add("Xdg Data Home",        xdgDirs.dataHome(false));
    table->add("Xdg Config Home",      xdgDirs.configHome(false));
    table->add("Xdg Data Dirs",        xdgDirs.dataDirs().join(":"));
    table->add("Xdg Cache Home",       xdgDirs.cacheHome(false));
    table->add("Xdg Runtime Home",     xdgDirs.runtimeDir());
    table->add("Xdg Autostart Dirs",   xdgDirs.autostartDirs().join(":"));
    table->add("Xdg Autostart Home",   xdgDirs.autostartHome(false));

}

