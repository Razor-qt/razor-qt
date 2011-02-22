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
#include "razorpanel.h"
#include <QDebug>
#include <QIcon>
#include <QLabel>
#include <QAction>
#include <QPushButton>
#include <QtCore/QFileInfo>
#include <QtCore/QLibrary>

class FakePlugin: public RazorPanelPlugin
{
public:
    FakePlugin(RazorPanel* panel, const QString& configId, QWidget *parent, const QString& objName, const QString& title)
        : RazorPanelPlugin(panel, configId, parent)
    {
        setWindowTitle(title);
        setObjectName(objName);
    }

};


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
RazorPanelPlugin* RazorPluginManager::loadPlugin(const QString& libraryFileName, const QString& configId, RazorPanel* panel)
{
    RazorPanelPlugin* plugin = 0;
    qDebug() << "RazorPluginManager: try to load " << libraryFileName;


    if (libraryFileName.endsWith("librazorpanel_mainmenu2.so"))
    {
        plugin = new FakePlugin(panel, configId, panel, "MainMenu", "Main Menu");

        QPushButton* mb = new QPushButton("Menu", plugin);

        //         mb->setMenu(mMenu);
        mb->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding));
        plugin->addWidget(mb);
        //         addPlugin(plugin);


    }


    if (libraryFileName.endsWith("librazorpanel_quicklaunch2.so"))
    {
        plugin = new FakePlugin(panel, configId, panel, "QuickLaunch", "Quick Launch");

        plugin->addAction(QIcon::fromTheme("kate"), "Kate");
        plugin->addAction(QIcon::fromTheme("utilities-terminal"), "Konsole");
        plugin->addAction(QIcon::fromTheme("kmail"), "kmail");
    }


    if (libraryFileName.endsWith("librazorpanel_taskmanager2.so"))
    {
        plugin = new FakePlugin(panel, configId, panel, "TaskBar", "TaskBar");

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


    if (libraryFileName.endsWith("librazorpanel_logoutmenu.so"))
    {

    }
    if (plugin)
    {
        append(plugin);
        return plugin;
    }

    // FAKE <<--------------------



    // check if the file exists. Probably debug only.
    QFileInfo fi(libraryFileName);
    if (!fi.exists())
    {
        qDebug() << "PLUGIN: MISSING FILE";
        return 0;
    }

    QLibrary * lib = new QLibrary(libraryFileName, panel);
    PluginInitFunction initFunc = (PluginInitFunction) lib->resolve("init");
    if (!initFunc)
    {
        qDebug() << "PLUGIN: plugin: MISSING init()";
        delete lib;
        return 0;
    }

    plugin = initFunc(panel, panel, configId);
    Q_ASSERT(plugin);
    // now add the plug into the panel's layout.
    // it's easier to do it here instead to handle it in plugin itself

    if (plugin)
    {
        append(plugin);
        qDebug() << "RazorPluginManager: plugin sacceffuly loaded";
    }
    return plugin;
}
