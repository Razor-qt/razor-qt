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


#include "plugin.h"
#include "irazorpanelplugin.h"
#include "razorpanel.h"
#include <QDebug>
#include <QProcessEnvironment>
#include <QStringList>
#include <QDir>
#include <QFileInfo>
#include <QPluginLoader>
#include <QGridLayout>
#include <QDialog>
#include <QEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QApplication>
#include <QtCore/QCryptographicHash>

#include <razorqt/razorsettings.h>
#include <qtxdg/xdgicon.h>

QColor Plugin::mMoveMarkerColor= QColor(255, 0, 0, 255);

/************************************************

 ************************************************/
Plugin::Plugin(const RazorPluginInfo &desktopFile, const QString &settingsFile, const QString &settingsGroup, RazorPanel *panel) :
    QFrame(panel),
    mDesktopFile(desktopFile),
    mPluginLoader(0),
    mPlugin(0),
    mPluginWidget(0),
    mAlignment(AlignLeft),
    mSettingsGroup(settingsGroup),
    mPanel(panel)
{

    mSettings = new RazorSettings(settingsFile, QSettings::IniFormat, this);
    connect(mSettings, SIGNAL(settingsChanged()), this, SLOT(settingsChanged()));
    mSettings->beginGroup(settingsGroup);

    mSettingsHash = calcSettingsHash();

    setWindowTitle(desktopFile.name());

    QStringList dirs;
    dirs << QProcessEnvironment::systemEnvironment().value("RAZORPANEL_PLUGIN_PATH").split(":");
    dirs << PLUGIN_DIR;

    QString baseName = QString("lib%1.so").arg(desktopFile.id());
    bool found = false;
    foreach(QString dirName, dirs)
    {
        QFileInfo fi(QDir(dirName), baseName);

        if (fi.exists())
        {
            found = true;
            if (loadLib(fi.absoluteFilePath()))
                break;
        }
    }

    if (!isLoaded())
    {
        if (!found)
            qWarning() << QString("Plugin %1 not found in the").arg(baseName) << dirs;

        return;
    }

    setObjectName(mPlugin->themeId() + "Plugin");
    QString s = mSettings->value("alignment").toString();

    // Retrun default value
    if (s.isEmpty())
    {
        mAlignment = (mPlugin->flags().testFlag(IRazorPanelPlugin::PreferRightAlignment)) ?
                    Plugin::AlignRight :
                    Plugin::AlignLeft;
    }
    else
    {
        mAlignment = (s.toUpper() == "RIGHT") ?
                    Plugin::AlignRight :
                    Plugin::AlignLeft;

    }

    if (mPluginWidget)
    {
        QGridLayout* layout = new QGridLayout(this);
        layout->setSpacing(0);
        layout->setMargin(0);
        layout->setContentsMargins(0, 0, 0, 0);
        setLayout(layout);
        layout->addWidget(mPluginWidget, 0, 0);
    }

    saveSettings();
}


/************************************************

 ************************************************/
Plugin::~Plugin()
{
    delete mPlugin;
    if (mPluginLoader)
    {
        mPluginLoader->unload();
        delete mPluginLoader;
    }
}

void Plugin::setAlignment(Plugin::Alignment alignment)
{
    mAlignment = alignment;
    saveSettings();
}


/************************************************

 ************************************************/
bool Plugin::loadLib(const QString &libraryName)
{
    mPluginLoader = new QPluginLoader(libraryName);

    if (!mPluginLoader->load())
    {
        qWarning() << mPluginLoader->errorString();
        return false;
    }

    QObject *obj = mPluginLoader->instance();
    if (!obj)
    {
        qWarning() << mPluginLoader->errorString();
        return false;
    }

    IRazorPanelPluginLibrary* pluginLib= qobject_cast<IRazorPanelPluginLibrary*>(obj);
    if (!pluginLib)
    {
        qWarning() << QString("Can't load plugin \"%1\". Plugin is not a IRazorPanelPluginLibrary.").arg(mPluginLoader->fileName());
        delete obj;
        return false;
    }

    IRazorPanelPluginStartupInfo startupInfo;
    startupInfo.settings = mSettings;
    startupInfo.desktopFile = &mDesktopFile;
    startupInfo.razorPanel = mPanel;

    mPlugin = pluginLib->instance(startupInfo);
    if (!mPlugin)
    {
        qWarning() << QString("Can't load plugin \"%1\". Plugin can't build IRazorPanelPlugin.").arg(mPluginLoader->fileName());
        delete obj;
        return false;
    }

    mPluginWidget = mPlugin->widget();
    if (mPluginWidget)
    {
        mPluginWidget->setObjectName(mPlugin->themeId());
    }
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    return true;
}


/************************************************

 ************************************************/
QByteArray Plugin::calcSettingsHash()
{
    QCryptographicHash hash(QCryptographicHash::Md5);
    QStringList keys = mSettings->allKeys();
    foreach (const QString &key, keys)
    {
        hash.addData(key.toUtf8());
        hash.addData(mSettings->value(key).toByteArray());
    }
    return hash.result();
}


/************************************************

 ************************************************/
void Plugin::settingsChanged()
{
    QByteArray hash = calcSettingsHash();
    if (mSettingsHash != hash)
    {
        mSettingsHash = hash;
        mPlugin->settingsChanged();
    }
}


/************************************************

 ************************************************/
void Plugin::saveSettings()
{
    mSettings->setValue("alignment", (mAlignment == AlignLeft) ? "Left" : "Right");
    mSettings->setValue("type", mDesktopFile.id());
    mSettings->sync();

}


/************************************************

 ************************************************/
void Plugin::x11EventFilter(XEvent *event)
{
    mPlugin->x11EventFilter(event);
}


/************************************************

 ************************************************/
void Plugin::contextMenuEvent(QContextMenuEvent *event)
{
    mPanel->showPopupMenu(this);
}


/************************************************

 ************************************************/
void Plugin::mousePressEvent(QMouseEvent *event)
{
    switch (event->button())
    {
    case Qt::LeftButton:
        mPlugin->activated(IRazorPanelPlugin::Trigger);
        break;

    case Qt::MidButton:
        mPlugin->activated(IRazorPanelPlugin::MiddleClick);
        break;

    default:
        break;
    }
}


/************************************************

 ************************************************/
void Plugin::mouseDoubleClickEvent(QMouseEvent*)
{
    mPlugin->activated(IRazorPanelPlugin::DoubleClick);
}


/************************************************

 ************************************************/
void Plugin::showEvent(QShowEvent *)
{
    if (mPluginWidget)
        mPluginWidget->adjustSize();
}


/************************************************

 ************************************************/
QMenu *Plugin::popupMenu() const
{

    QMenu* menu = new QMenu(windowTitle());

    if (mPlugin->flags().testFlag(IRazorPanelPlugin::HaveConfigDialog))
    {
        QAction* configAction = new QAction(tr("Configure"), menu);
        menu->addAction(configAction);
        connect(configAction, SIGNAL(triggered()), this, SLOT(showConfigureDialog()));
    }

    QAction* moveAction = new QAction(XdgIcon::fromTheme("transform-move"), tr("Move"), menu);
    menu->addAction(moveAction);
    connect(moveAction, SIGNAL(triggered()), this, SIGNAL(startMove()));

    menu->addSeparator();

    QAction* removeAction = new QAction(XdgIcon::fromTheme("dialog-close"), tr("Remove"), menu);
    menu->addAction(removeAction);
    connect(removeAction, SIGNAL(triggered()), this, SLOT(requestRemove()));

    return menu;
}


/************************************************

 ************************************************/
bool Plugin::isSeparate() const
{
   return mPlugin->isSeparate();
}


/************************************************

 ************************************************/
bool Plugin::isExpandable() const
{
    return mPlugin->isExpandable();
}


/************************************************

 ************************************************/
void Plugin::realign()
{
    if (mPlugin)
        mPlugin->realign();
}


/************************************************

 ************************************************/
void Plugin::showConfigureDialog()
{
    static QWeakPointer<QDialog> ref;
    QDialog *dialog = ref.data();

    if (!dialog)
    {
        dialog = mPlugin->configureDialog();
        ref = dialog;
        connect(this, SIGNAL(destroyed()), dialog, SLOT(close()));
    }

    if (!dialog)
        return;

    dialog->show();
    dialog->raise();
    dialog->activateWindow();
}


/************************************************

 ************************************************/
void Plugin::requestRemove()
{
    emit remove();
    deleteLater();
}
