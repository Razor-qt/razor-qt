/********************************************************************
  Copyright: 2010 Alexander Sokoloff <sokoloff.a@gmail.ru>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License.
  version 2 as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*********************************************************************/

#include "razorpluginmanager.h"
#include "razorpanelplugin.h"
#include <QDebug>
#include <QIcon>
#include <QLabel>
#include <QAction>
#include <QPushButton>

/************************************************

 ************************************************/
RazorPluginManager::RazorPluginManager() :
    QList<RazorPanelPlugin*>()
{
}


/************************************************

 ************************************************/
RazorPluginManager::~RazorPluginManager()
{
    qDeleteAll(begin(), end());
}


/************************************************

 ************************************************/
RazorPanelPlugin* RazorPluginManager::loadPlugin(const QString& libraryFileName)
{
    RazorPanelPlugin* plugin = 0;

    if (libraryFileName.endsWith("librazorpanel_mainmenu.so"))
    {
        plugin = new RazorPanelPlugin();
        plugin->setWindowTitle("Main Menu");
        plugin->setObjectName("MainMenu");


        QPushButton* mb = new QPushButton("Menu", plugin);

        //         mb->setMenu(mMenu);
        mb->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding));
        plugin->addWidget(mb);
        //         addPlugin(plugin);


    }


    if (libraryFileName.endsWith("librazorpanel_quicklaunch.so"))
    {
        plugin = new RazorPanelPlugin();
        plugin->setObjectName("QuickLaunch");
        plugin->setWindowTitle("Quick Launch");

        plugin->addAction(QIcon::fromTheme("kate"), "Kate");
        plugin->addAction(QIcon::fromTheme("utilities-terminal"), "Konsole");
        plugin->addAction(QIcon::fromTheme("kmail"), "kmail");
    }


    if (libraryFileName.endsWith("librazorpanel_taskmanager.so"))
    {
        plugin = new RazorPanelPlugin();
        plugin->setObjectName("TaskBar");
        plugin->setWindowTitle("TaskBar");

        plugin->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        QAction* a;
        plugin->addAction(QIcon::fromTheme("kate"), "Application");

        a = plugin->addAction(QIcon::fromTheme("utilities-terminal"), "Active App");
        a->setCheckable(true);
        a->setChecked(true);

        plugin->addAction(QIcon::fromTheme("kmail"), "kmail");
    }


    if (libraryFileName.endsWith("librazorpanel_traybar.so"))
    {

    }


    if (libraryFileName.endsWith("librazorpanel_clock.so"))
    {
        plugin = new RazorPanelPlugin();
        plugin->setObjectName("Clock");
        plugin->setWindowTitle("Clock");

        QLabel* clock = new QLabel();
        clock->setAlignment(Qt::AlignCenter);
        clock->setText("19:41<br><small>Thu,3 of Feb</small>");
        plugin->addWidget(clock);
    }


    if (libraryFileName.endsWith("librazorpanel_logoutmenu.so"))
    {

    }


    if (plugin)
        append(plugin);

    return plugin;
}
