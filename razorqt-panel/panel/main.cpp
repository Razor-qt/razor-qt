/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
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


#include <QtGui/QApplication>
#include <QtGui/QIcon>
#include <QtCore/QDebug>
#include <QtCore/QLibraryInfo>
#include <csignal>

#include "razorpanelapplication.h"
#include "razorpanel.h"
#include "razortranslate.h"

/*! The razor-panel is the panel of Razor-qt.
  Usage: razor-panel [CONFIG_ID]
    CONFIG_ID      Section name in config file ~/.razor/panel.conf
                   (default main)
 */

void termSignalHandler(int)
{
    qApp->quit();
}


int main(int argc, char *argv[])
{
    RazorPanelApplication *app = new RazorPanelApplication(argc, argv);

    TRANSLATE_APP;

    // Read command line arguments .......................
    // The first argument is config file name.
    QString configFile = "panel";
    if (app->arguments().count() > 1)
    {
        configFile = app->arguments().at(1);
        if (configFile.endsWith(".conf"))
            configFile.chop(5);
    }

    RazorPanel *panel = new RazorPanel(configFile);
    app->setPanel(panel);

    //Setup Unix signal handlers
    struct sigaction term;
    term.sa_handler = termSignalHandler;
    sigemptyset(&term.sa_mask);
    term.sa_flags |= SA_RESTART;
    sigaction(SIGTERM, &term, 0);
    sigaction(SIGINT,  &term, 0);

    panel->show();
    bool res = app->exec();

    app->setPanel(0);
    delete panel;
    app->deleteLater();
    return res;
}
