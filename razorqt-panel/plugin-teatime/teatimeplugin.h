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

#ifndef TEATIMEPLUGIN_H
#define TEATIMEPLUGIN_H

#include "../panel/irazorpanelplugin.h"
#include <QtGui/QToolButton>

class TeaTimePlugin: public QObject, public IRazorPanelPlugin
{
    Q_OBJECT
public:
    TeaTimePlugin(const IRazorPanelPluginStartupInfo &startupInfo);

    virtual QWidget *widget();
    virtual QString themeId() const { return "TeaTime"; }
    virtual IRazorPanelPlugin::Flags flags() const { return  PreferRightAlignment; }

private:
    QToolButton mButton;
};

/*
class TeaTimePlugin: public QObject, public IRazorPanelPlugin
{
    Q_OBJECT
public:
    TeaTimePlugin(const IRazorPanelPluginStartupInfo &startupInfo);
    void init(const IRazorPanelPluginStartupInfo &startupInfo) {}

    QString themeId() const { return "TeaTime"; }
    QWidget *widget();

private slots:
    void showMessage();

private:
    QToolButton mButton;
};
*/
class TeaTimePluginLibrary: public QObject, public IRazorPanelPluginLibrary
{
    Q_OBJECT
    Q_INTERFACES(IRazorPanelPluginLibrary)
public:
    IRazorPanelPlugin *instance(const IRazorPanelPluginStartupInfo &startupInfo)
    {
        return new TeaTimePlugin(startupInfo);
    }
};

#endif // TEATIMEPLUGIN_H
