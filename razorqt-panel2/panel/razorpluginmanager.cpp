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

    qDebug() << "RazorPanel: try to load " << libraryFileName;

    QLibrary * lib = new QLibrary(libraryFileName);
    PluginInitFunction initFunc = (PluginInitFunction) lib->resolve("init");
    if (!initFunc)
    {
        qDebug() << lib->errorString();
        delete lib;
        return 0;
    }

    plugin = initFunc(panel, panel, configId);
    Q_ASSERT(plugin);

    if (plugin)
    {
        lib->setParent(plugin);
        append(plugin);
        qDebug() << "    * Plugin loaded.";
    }
    return plugin;
}

