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

#include "razorthemeconfig.h"
#include "ui_razorthemeconfig.h"
#include <QtGui/QTreeWidget>
#include <QDebug>

RazorThemeConfig::RazorThemeConfig(RazorSettings *settings, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RazorThemeConfig),
    mSettings(settings)
{
    ui->setupUi(this);

    connect(ui->razorThemeList, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this, SLOT(razorThemeSelected(QTreeWidgetItem*,int)));


    QList<RazorTheme> themes = RazorTheme::allThemes();
    foreach(RazorTheme theme, themes)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(QStringList(theme.name()));
        if (!theme.previewImage().isEmpty())
        {
            item->setIcon(0, QIcon(theme.previewImage()));
        }
        item->setSizeHint(0, QSize(42,42)); // make icons non-cropped
        item->setData(0, Qt::UserRole, theme.name());
        ui->razorThemeList->addTopLevelItem(item);
    }

    initControls();
}


RazorThemeConfig::~RazorThemeConfig()
{
    delete ui;
}


void RazorThemeConfig::initControls()
{
    QString currentTheme = mSettings->value("theme").toString();

    QTreeWidgetItemIterator it(ui->razorThemeList);
    while (*it) {
        if ((*it)->data(0, Qt::UserRole).toString() == currentTheme)
        {
            ui->razorThemeList->setCurrentItem((*it));
            break;
        }
        ++it;
    }

    update();
}


void RazorThemeConfig::razorThemeSelected(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);
    if (!item)
        return;
    mSettings->setValue("theme", item->data(0, Qt::UserRole));
}
