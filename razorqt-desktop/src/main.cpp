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

#include <QApplication>
#include <QLibrary>
#include <QtDebug>
#include <QIcon>

#include <razorqt/razorsettings.h>
#include <qtxdg/xdgicon.h>
#include "desktopplugin.h"
#include "razordesktopapplication.h"



int main (int argc, char* argv[])
{
    RazorDesktopApplication app(argc,argv);
    XdgIcon::setThemeName(RazorSettings::globalSettings()->value("icon_theme").toString());
    app.setWindowIcon(QIcon(QString(SHARE_DIR) + "/graphics/razor_logo.png"));
    
    RazorSettings config("desktop");

    app.setStyleSheet(razorTheme->qss("desktop"));
    
    QString configId(config.value("desktop", "razor").toString());
    QString libraryFileName = QString(DESKTOP_PLUGIN_DIR) + "libdesktop-" + configId + ".so";

    qDebug() << "RazorDesktop: try to load " << libraryFileName;

    QLibrary * lib = new QLibrary(libraryFileName);
    PluginInitFunction initFunc = (PluginInitFunction) lib->resolve("init");
    if (!initFunc)
    {
        qDebug() << lib->errorString();
        delete lib;
        return 0;
    }

    DesktopPlugin * plugin = initFunc(configId, &config);
    Q_ASSERT(plugin);

    if (plugin)
    {
        //lib->setParent(plugin);
        qDebug() << "    * Plugin loaded.";
        qDebug() << plugin->info();
    }

    app.setDesktopPlugin(plugin);

    return app.exec();
}

