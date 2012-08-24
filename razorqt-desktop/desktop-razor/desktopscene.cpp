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

#include <QtCore/QLibrary>
#include <QtCore/QUuid>
#include <QtCore/QTimer>
#include <QtGui/QMessageBox>
#include <QtGui/QApplication>
#include <QtGui/QGraphicsSceneContextMenuEvent>

#include <qtxdg/xdgdirs.h>
#include <qtxdg/xdgicon.h>
#include <razorqt/powermanager.h>
#include <razorqt/screensaver.h>
#include <qtxdg/xdgmenuwidget.h>
#include <razorqt/razoraboutdlg.h>
#include <razorqt/addplugindialog/addplugindialog.h>

#include "desktopscene.h"
#include "workspacemanager.h"

#include <razorqt/xfitman.h>


DesktopScene::DesktopScene(QObject * parent)
    : QGraphicsScene(parent),
      m_wheelDesktopSwitch(false),
      m_menu(0),
      m_activePlugin(0)
{
    m_power = new PowerManager(this);
    m_screenSaver = new ScreenSaver(this);

    DesktopConfig::instance()->config->beginGroup("razor");
    QStringList plugins =  DesktopConfig::instance()->config->value("plugins").toStringList();
    m_menuFile = DesktopConfig::instance()->config->value("menu_file", "").toString();
    m_wheelDesktopSwitch = DesktopConfig::instance()->config->value("mouse_wheel_desktop_switch", false).toBool();
    DesktopConfig::instance()->config->endGroup();
    if (m_menuFile.isEmpty())
        m_menuFile = XdgMenu::getMenuFileName();

    m_xdgMenu.setEnvironments(QStringList() << "X-RAZOR" << "Razor");
    bool res = m_xdgMenu.read(m_menuFile);
    connect(&m_xdgMenu, SIGNAL(changed()), this, SLOT(buildMenu()));

    if (res)
    {
        QTimer::singleShot(1000, this, SLOT(buildMenu()));
    }
    else
    {
        QMessageBox::warning(0, "Menu Parse error", m_xdgMenu.errorString());
        return;
    }

    m_actArrangeWidgets = new QAction(tr("Unlock Desktop..."), this);
    m_actArrangeWidgets->setIcon(XdgIcon::fromTheme("object-locked"));
    m_actArrangeWidgets->setCheckable(true);
    connect(m_actArrangeWidgets, SIGNAL(toggled(bool)),
            this, SLOT(arrangeWidgets(bool)));

    m_actAddNewPlugin = new QAction(tr("Add New Desktop Widget..."), this);
    connect(m_actAddNewPlugin, SIGNAL(triggered()),
            this, SLOT(showAddPluginDialog()));

    m_actRemovePlugin = new QAction(tr("Remove Plugin..."), this);
    connect(m_actRemovePlugin, SIGNAL(triggered()),
            this, SLOT(removePlugin()));

    m_actConfigurePlugin = new QAction(tr("Configure Plugin..."), this);
    connect(m_actConfigurePlugin, SIGNAL(triggered()),
            this, SLOT(configurePlugin()));

    m_actSetbackground = new QAction(tr("Set Desktop Background..."), this);
    m_actSetbackground->setIcon(XdgIcon::fromTheme("preferences-desktop-wallpaper"));
    connect(m_actSetbackground, SIGNAL(triggered()),
            this, SLOT(setDesktopBackground()));

    m_actAbout = new QAction(tr("About Razor..."), this);
    m_actAbout->setIcon(XdgIcon::fromTheme("help-browser"));
    connect(m_actAbout, SIGNAL(triggered()), this, SLOT(about()));

    // load plugins
    QStringList desktopDirs = pluginDesktopDirs();
    foreach (QString configId, plugins)
    {
        DesktopConfig::instance()->config->beginGroup(configId);
        QString libName(DesktopConfig::instance()->config->value("plugin", "").toString());

        qreal x = DesktopConfig::instance()->config->value("x", 10.0).toReal();
        qreal y = DesktopConfig::instance()->config->value("y", 10.0).toReal();
        qreal w = DesktopConfig::instance()->config->value("w", 10.0).toReal();
        qreal h = DesktopConfig::instance()->config->value("h", 10.0).toReal();
        QPointF position(x, y);
        QSizeF size(w, h);
        DesktopConfig::instance()->config->endGroup();

        RazorPluginInfoList list = RazorPluginInfo::search(desktopDirs, "RazorDesktop/Plugin", QString("%1.desktop").arg(libName));
        if( !list.count())
        {
            qWarning() << QString("Plugin \"%1\" not found.").arg(libName);
            continue;
        }
        QLibrary* lib = loadPluginLib(list.first());
        if (!lib)
        {
            qWarning() << "RazorWorkSpace::setConfig() Library" << libName << "is not loaded";
            continue;
        }

        DesktopWidgetPlugin * item = m_plugins[configId];
        if (!item)
        {
            item = loadPlugin(lib, configId);
            m_plugins.insert(configId, item);
        }

        item->setSizeAndPosition(position, size);
    }
}

void DesktopScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *e)
{
    if (m_menu)
    {
    	m_activePlugin = getPluginFromPoint(e->scenePos());
    	if (m_activePlugin)
    	{
    		m_actRemovePlugin->setVisible(true);
    		m_actConfigurePlugin->setVisible(true);
    	}
    	else
    	{
    		m_actRemovePlugin->setVisible(false);
    		m_actConfigurePlugin->setVisible(false);
    	}
        m_menu->exec(QCursor::pos());
        e->accept();
        return;
    }
    QGraphicsScene::contextMenuEvent(e);
}

DesktopWidgetPlugin * DesktopScene::getPluginFromItem(QGraphicsItem * item)
{
    if (!item)
    {
    	qDebug() << "DesktopScene::getPluginFromItem - no item";
        return 0;
    }
    QGraphicsItem *top = item->topLevelItem();
    if (top->zValue() != DesktopWidgetPlugin::ZVAL())
    {
    	qDebug() << "DesktopScene::getPluginFromItem - no ZValue" << top->zValue();
    	return 0;
    }
//    qDebug() << "DesktopScene::getPliginFromItem" << top << static_cast<DesktopWidgetPlugin*>(top);
    return static_cast<DesktopWidgetPlugin*>(top);
}

DesktopWidgetPlugin * DesktopScene::getPluginFromPoint(const QPointF &point)
{
    return getPluginFromItem(itemAt(point));
}

void DesktopScene::buildMenu()
{
    XdgMenuWidget *menu = new XdgMenuWidget(m_xdgMenu, "");

    menu->setObjectName("TopLevelMainMenu");

    menu->addSeparator();
    menu->addAction(m_actArrangeWidgets);
    menu->addAction(m_actSetbackground);
    menu->addAction(m_actAddNewPlugin);
    menu->addAction(m_actConfigurePlugin);
    menu->addAction(m_actRemovePlugin);
    menu->addAction(m_actAbout);
    menu->addSeparator();
    menu->addActions(m_power->availableActions());
    menu->addSeparator();
    menu->addActions(m_screenSaver->availableActions());
#ifdef DEBUG
    menu->addAction("Exit (debug only)", qApp, SLOT(quit()));
#endif

    QMenu *prevMenu = m_menu;
    m_menu = menu;
    delete prevMenu;
}

void DesktopScene::wheelEvent(QGraphicsSceneWheelEvent * e)
{
    if (!m_wheelDesktopSwitch)
    {
        QGraphicsScene::wheelEvent(e);
        return;
    }
    DesktopWidgetPlugin * plug = getPluginFromPoint(e->scenePos());
    if (!plug) {

        int max = xfitMan().getNumDesktop() - 1;
        int delta = e->delta() > 0 ? 1 : -1;
        int current = xfitMan().getActiveDesktop() + delta;

        if (current > max)
            current = 0;
        else if (current < 0)
            current = max;

        xfitMan().setActiveDesktop(current);
    }

    QGraphicsScene::wheelEvent(e);
}

void DesktopScene::configurePlugin()
{
	qDebug() << "DesktopScene::configurePlugin" << m_activePlugin;
	m_activePlugin->configure();
}

void DesktopScene::about()
{
    RazorAboutDLG dia;
}

void DesktopScene::showAddPluginDialog()
{
    AddPluginDialog* dlg = findChild<AddPluginDialog*>();

    if (!dlg)
    {
        dlg = new AddPluginDialog(pluginDesktopDirs(), "RazorDesktop/Plugin", "*");
//        dlg->move(sceneRect().center() - dlg->sceneRect().center());
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        connect(dlg, SIGNAL(pluginSelected(const RazorPluginInfo&)),
        		this, SLOT(addPlugin(const RazorPluginInfo&)));
    }

    dlg->exec();
}

void DesktopScene::addPlugin(const RazorPluginInfo &pluginInfo)
{
    QLibrary* lib = loadPluginLib(pluginInfo);
    if (!lib)
        return;
    QString pluginID = QString("%1_%2").arg(pluginInfo.id()).arg(QUuid::createUuid().toString());
    DesktopWidgetPlugin * plugin = loadPlugin(lib, pluginID);

    // "clever" positioning
    QSizeF size(200, 200);
    int x = 10, y = 10;
    int xmax = sceneRect().width()-size.width()-x;
    int ymax = sceneRect().height()-size.height()-y;
    while (true)
    {
        bool placeIt = true;
        plugin->setSizeAndPosition(QPointF(x, y), size);
        foreach (DesktopWidgetPlugin* i, m_plugins.values())
        {
            if (plugin->collidesWithItem(i))
            {
                placeIt = false;
                break;
            }
            placeIt = true;
        }

        if (placeIt)
        {
        	m_plugins[pluginID] = plugin;
        	save();
            break;
        }

        x += 20;
        if (x >= xmax)
        {
            x = 0;
            y += 20;
        }
        if (y >= ymax)
        {
            QMessageBox::information(0, tr("New Desktop Widget"),
                                        tr("There is no free space to add new desktop widget"));
            if (plugin)
            {
                m_activePlugin = plugin;
                removePlugin(false);
                //delete item;
            }
            // TODO/FIXME: check for mem leaks one day...
            if (plugin) delete plugin;
            if (lib) delete lib;
            break;
        }
    } // while
}

void DesktopScene::removePlugin(bool ask)
{
    if (ask && QMessageBox::question(0,
                              tr("Remove Desktop Widget?"),
                              tr("Really remove this widget '%1'?").arg(m_activePlugin->instanceInfo()),
                            QMessageBox::Yes, QMessageBox::No)
        == QMessageBox::No)
    {
        return;
    }

    m_activePlugin->removeConfig();
    removeItem(m_activePlugin);
    m_plugins.remove(m_activePlugin->configId());
    m_activePlugin->deleteLater();
    save();
}

DesktopWidgetPlugin * DesktopScene::loadPlugin(QLibrary * lib, const QString & configId)
{
    Q_ASSERT(lib);
    DesktopWidgetInitFunction initFunc = (DesktopWidgetInitFunction) lib->resolve("init");
    if (!initFunc)
    {
        qWarning() << lib->errorString();
        delete lib;
    }
    else
    {
        DesktopWidgetPlugin * plugin = initFunc(this, configId, DesktopConfig::instance()->config);
        Q_ASSERT(plugin);
        addItem(plugin);
        return plugin;
    }
    return 0;
}

QStringList DesktopScene::pluginDesktopDirs()
{
    QStringList dirs;
    dirs << QString(getenv("RAZORQT_DESKTOP_PLUGINS_DIR")).split(':', QString::SkipEmptyParts);
    dirs << QString("%1/%2").arg(XdgDirs::dataHome(), "/razor/razor-desktop");
    dirs << PLUGIN_DESKTOP_FILES_DIR;
    return dirs;
}

QLibrary* DesktopScene::loadPluginLib(const RazorPluginInfo &pluginInfo)
{
    QLibrary* lib = 0;

    if (getenv("RAZORQT_DESKTOP_PLUGINS_SO_DIR"))
        lib = pluginInfo.loadLibrary(getenv("RAZORQT_DESKTOP_PLUGINS_SO_DIR"));

    if (!lib)
        lib = pluginInfo.loadLibrary(DESKTOP_PLUGIN_DIR);

    return lib;
}

void DesktopScene::arrangeWidgets(bool enable)
{
    m_actArrangeWidgets->setIcon(XdgIcon::fromTheme(enable ? "object-unlocked" : "object-locked"));
    m_actArrangeWidgets->setText(enable ? tr("Lock Desktop...") : tr("Unlock Desktop..."));
    foreach(DesktopWidgetPlugin *plug, m_plugins)
        plug->setEditable(enable);
}

void DesktopScene::save()
{
	QMapIterator<QString, DesktopWidgetPlugin*> it(m_plugins);
	while (it.hasNext())
	{
		it.next();
		it.value()->save();
	}

    DesktopConfig::instance()->config->beginGroup("razor");
    DesktopConfig::instance()->config->setValue("plugins", QStringList(m_plugins.keys()));
    DesktopConfig::instance()->config->setValue("menu_file", m_menuFile);
    DesktopConfig::instance()->config->setValue("mouse_wheel_desktop_switch", m_wheelDesktopSwitch);
    DesktopConfig::instance()->config->endGroup();

    emit saveConfig();
}

void DesktopScene::setDesktopBackground()
{
	QPoint p = QCursor::pos();
	foreach(QGraphicsView* i, views())
	{
		QPoint wp = i->pos();
		QSize sp = i->size();
		if (p.x() >= wp.x()
			&& p.y() >= wp.y()
			&& p.x() <= wp.x()+sp.width()
			&& p.y() <= wp.y()+sp.height()
		   )
		{
			qDebug() << "DesktopScene::setDesktopBackground for" << i;
			qobject_cast<RazorWorkSpace*>(i)->setDesktopBackground();
			break;
		}
	}
}
