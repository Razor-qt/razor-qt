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

#ifndef RAZORTHEMECONFIG_H
#define RAZORTHEMECONFIG_H

#include <QtGui/QWidget>
#include <razorqt/razorsettings.h>

class QTreeWidgetItem;

namespace Ui {
    class RazorThemeConfig;
}

class RazorThemeConfig : public QWidget
{
    Q_OBJECT

public:
    explicit RazorThemeConfig(RazorSettings *settings, QWidget *parent = 0);
    ~RazorThemeConfig();

public slots:
    void initControls();

private slots:
    void razorThemeSelected(QTreeWidgetItem* item, int column);

private:
    Ui::RazorThemeConfig *ui;
    RazorSettings *mSettings;
};

#endif // RAZORTHEMECONFIG_H
