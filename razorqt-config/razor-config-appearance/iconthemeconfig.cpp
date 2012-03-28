/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
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
 
#include "iconthemeconfig.h"

#include <qtxdg/xdgdesktopfile.h>
#include <qtxdg/xdgicon.h>
#include <razorqt/razorsettings.h>
#include <QtCore/QStringList>
#include <QtGui/QIcon>
#include <QtCore/QDebug>


IconThemeConfig::IconThemeConfig(RazorSettings* settings):
    m_settings(settings)
{
    setupUi(this);

    initIconsThemes();
    initControls();
    connect(iconThemeList, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this, SLOT(iconThemeSelected(QTreeWidgetItem*,int)));

    connect(RazorSettings::globalSettings(), SIGNAL(settigsChanged()),
            this, SLOT(update()));
}


void IconThemeConfig::initIconsThemes()
{
    QStringList processed;
    QStringList baseDirs = QIcon::themeSearchPaths();

    foreach (QString baseDirName, baseDirs)
    {
        QDir baseDir(baseDirName);
        if (!baseDir.exists())
            continue;

        QFileInfoList dirs = baseDir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot, QDir::Name);
        foreach (QFileInfo dir, dirs)
        {
            if (!processed.contains(dir.canonicalFilePath()))
            {
                processed << dir.canonicalFilePath();

                IconThemeInfo theme(QDir(dir.canonicalFilePath()));
                if (theme.isValid() && (!theme.isHidden()))
                {
                    QTreeWidgetItem *item = new QTreeWidgetItem(iconThemeList);
                    item->setSizeHint(0, QSize(42,42)); // make icons non-cropped
                    item->setData(0, Qt::UserRole, theme.name());

                    item->setIcon(0, theme.icon("document-open"));
                    item->setIcon(1, theme.icon("document-new"));
                    item->setIcon(2, theme.icon("edit-undo"));
                    item->setIcon(3, theme.icon("media-playback-start"));

                    item->setText(4, theme.comment().isEmpty() ? theme.text() : theme.text() + " ( " + theme.comment() + " )");
                }
            }
        }
    }

    iconThemeList->setColumnCount(5);
    for (int i=0; i<iconThemeList->header()->count()-1; ++i)
    {
        iconThemeList->resizeColumnToContents(i);
    }
}


void IconThemeConfig::initControls()
{
    QString currentTheme = RazorSettings::globalSettings()->value("icon_theme").toString();
    XdgIcon::setThemeName(currentTheme);
    QTreeWidgetItemIterator it(iconThemeList);
    while (*it) {
        if ((*it)->data(0, Qt::UserRole).toString() == currentTheme)
        {
            iconThemeList->setCurrentItem((*it));
            break;
        }
        ++it;
    }

    update();
}


IconThemeConfig::~IconThemeConfig()
{
}


void IconThemeConfig::iconThemeSelected(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    QString theme = item->data(0, Qt::UserRole).toString();
    if (!theme.isEmpty())
    {
        XdgIcon::setThemeName(theme);
        m_settings->setValue("icon_theme",  theme);
        m_settings->sync();
    }
}


