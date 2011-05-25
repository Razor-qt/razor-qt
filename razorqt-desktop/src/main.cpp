/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * https://sourceforge.net/projects/razor-qt/
 *
 * Copyright: 2010-2011 Razor team
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

#include <QApplication>
#include <QLibrary>
#include <QtDebug>
#include <QIcon>

#include <razorqt/readsettings.h>
#include "desktopplugin.h"



int main (int argc, char* argv[])
{
    //TODO make singleton QApplication that does all this shit, then make razorsettings and stylesettings available and fix all the other stuff with it

    QApplication app(argc,argv);
    app.setWindowIcon(QIcon(QString(SHARE_DIR) + "/graphics/razor_logo.png"));
    
    ReadSettings s("razor");
    ReadTheme theme(s.settings()->value("theme", "").toString());
    ReadSettings config("desktop");

    app.setStyleSheet(theme.qss());
    
    QString configId(config.settings()->value("desktop", "razor").toString());
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

    DesktopPlugin * plugin = initFunc(configId, &config, &theme);
    Q_ASSERT(plugin);

    if (plugin)
    {
        //lib->setParent(plugin);
        qDebug() << "    * Plugin loaded.";
        qDebug() << plugin->info();
    }

    return app.exec();
}

