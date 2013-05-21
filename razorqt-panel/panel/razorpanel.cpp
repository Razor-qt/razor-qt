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


#include "razorpanel.h"
#include "razorpanellimits.h"
#include "irazorpanelplugin.h"
#include "razorpanelapplication.h"
#include "razorpanellayout.h"
#include "config/configpaneldialog.h"
#include "popupmenu.h"
#include "plugin.h"
#include <razorqt/addplugindialog/addplugindialog.h>
#include <razorqt/razorsettings.h>
#include <razorqt/razorplugininfo.h>

#include <razorqt/xfitman.h>

#include <QtCore/QDebug>
#include <QtGui/QDesktopWidget>
#include <QtGui/QMenu>
#include <qtxdg/xdgicon.h>

#include <qtxdg/xdgdirs.h>


// Config keys and groups
#define CFG_KEY_SCREENNUM   "desktop"
#define CFG_KEY_POSITION    "position"
#define CFG_KEY_LINESIZE    "lineSize"
#define CFG_KEY_LINECNT     "lineCount"
#define CFG_KEY_LENGTH      "width"
#define CFG_KEY_PERCENT     "width-percent"
#define CFG_KEY_ALIGNMENT   "alignment"
#define CFG_KEY_PLUGINS "plugins"


/************************************************
 Returns the Position by the string.
 String is one of "Top", "Left", "Bottom", "Right", string is not case sensitive.
 If the string is not correct, returns defaultValue.
 ************************************************/
IRazorPanel::Position RazorPanel::strToPosition(const QString& str, IRazorPanel::Position defaultValue)
{
    if (str.toUpper() == "TOP")    return RazorPanel::PositionTop;
    if (str.toUpper() == "LEFT")   return RazorPanel::PositionLeft;
    if (str.toUpper() == "RIGHT")  return RazorPanel::PositionRight;
    if (str.toUpper() == "BOTTOM") return RazorPanel::PositionBottom;
    return defaultValue;
}


/************************************************
 Return  string representation of the position
 ************************************************/
QString RazorPanel::positionToStr(IRazorPanel::Position position)
{
    switch (position)
    {
        case RazorPanel::PositionTop:    return QString("Top");
        case RazorPanel::PositionLeft:   return QString("Left");
        case RazorPanel::PositionRight:  return QString("Right");
        case RazorPanel::PositionBottom: return QString("Bottom");
    }

    return QString();
}


/************************************************

 ************************************************/
RazorPanel::RazorPanel(const QString &configGroup, QWidget *parent) :
    QFrame(parent),
    mConfigGroup(configGroup),
    mLineSize(22),
    mLineCount(1)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_X11NetWmWindowTypeDock);
    setAttribute(Qt::WA_AlwaysShowToolTips);
    setWindowTitle("Razor Panel");
    setObjectName(QString("RazorPanel %1").arg(configGroup));

    mLayout = new RazorPanelLayout(this);
    connect(mLayout, SIGNAL(pluginMoved()), this, SLOT(pluginMoved()));
    setLayout(mLayout);
    mLayout->setLineCount(mLineCount);

    mDelaySave.setSingleShot(true);
    mDelaySave.setInterval(SETTINGS_SAVE_DELAY);
    connect(&mDelaySave, SIGNAL(timeout()), this, SLOT(saveSettings()));

    connect(QApplication::desktop(), SIGNAL(resized(int)), this, SLOT(screensChangeds()));
    connect(QApplication::desktop(), SIGNAL(resized(int)), this, SLOT(realign()));
    connect(QApplication::desktop(), SIGNAL(screenCountChanged(int)), this, SLOT(screensChangeds()));
    connect(RazorSettings::globalSettings(), SIGNAL(settingsChanged()), this, SLOT(update()));
    connect(razorApp, SIGNAL(themeChanged()), this, SLOT(realign()));

    mSettings = new RazorSettings("panel", this);
    readSettings();
    loadPlugins();
    
    show();
}


/************************************************

 ************************************************/
void RazorPanel::readSettings()
{
    // Read settings ......................................
    mSettings->beginGroup(mConfigGroup);

    // By default we are using size & count from theme.
    setLineSize(mSettings->value(CFG_KEY_LINESIZE, mLineSize).toInt());
    setLineCount(mSettings->value(CFG_KEY_LINECNT, mLineCount).toInt());

    setLength(mSettings->value(CFG_KEY_LENGTH, 100).toInt(),
              mSettings->value(CFG_KEY_PERCENT, true).toBool());

    setPosition(mSettings->value(CFG_KEY_SCREENNUM, QApplication::desktop()->primaryScreen()).toInt(),
                strToPosition(mSettings->value(CFG_KEY_POSITION).toString(), PositionBottom));

    setAlignment(RazorPanel::Alignment(mSettings->value(CFG_KEY_ALIGNMENT, mAlignment).toInt()));
    mSettings->endGroup();
}


/************************************************

 ************************************************/
void RazorPanel::saveSettings(bool later)
{
    mDelaySave.stop();
    if (later)
    {
        mDelaySave.start();
        return;
    }

    QStringList pluginsList;

    mSettings->beginGroup(mConfigGroup);

    foreach (const Plugin *plugin, mPlugins)
    {
        pluginsList << plugin->settingsGroup();
    }

    if (pluginsList.isEmpty())
    {
        mSettings->setValue(CFG_KEY_PLUGINS, "");
    }
    else
    {
        mSettings->setValue(CFG_KEY_PLUGINS, pluginsList);
    }

    mSettings->setValue(CFG_KEY_LINESIZE, mLineSize);
    mSettings->setValue(CFG_KEY_LINECNT,  mLineCount);

    mSettings->setValue(CFG_KEY_LENGTH,   mLength);
    mSettings->setValue(CFG_KEY_PERCENT,  mLengthInPercents);

    mSettings->setValue(CFG_KEY_SCREENNUM, mScreenNum);
    mSettings->setValue(CFG_KEY_POSITION, positionToStr(mPosition));

    mSettings->setValue(CFG_KEY_ALIGNMENT, mAlignment);

    mSettings->endGroup();
}


/************************************************

 ************************************************/
void RazorPanel::screensChangeds()
{
    if (! canPlacedOn(mScreenNum, mPosition))
        setPosition(findAvailableScreen(mPosition), mPosition);
}


/************************************************

 ************************************************/
RazorPanel::~RazorPanel()
{
    mLayout->setEnabled(false);
    // do not save settings because of "user deleted panel" functionality saveSettings();
    qDeleteAll(mPlugins);
}


/************************************************

 ************************************************/
void RazorPanel::show()
{
    QWidget::show();
    xfitMan().moveWindowToDesktop(this->effectiveWinId(), -1);
}


/************************************************

 ************************************************/
QStringList pluginDesktopDirs()
{
    QStringList dirs;
    dirs << QString(getenv("RAZORQT_PANEL_PLUGINS_DIR")).split(':', QString::SkipEmptyParts);
    dirs << QString("%1/%2").arg(XdgDirs::dataHome(), "/razor/razor-panel");
    dirs << PLUGIN_DESKTOPS_DIR;
    return dirs;
}


/************************************************

 ************************************************/
void RazorPanel::loadPlugins()
{
    QStringList desktopDirs = pluginDesktopDirs();
    mSettings->beginGroup(mConfigGroup);
    QStringList sections = mSettings->value(CFG_KEY_PLUGINS).toStringList();
    mSettings->endGroup();

    foreach (QString sect, sections)
    {
        QString type = mSettings->value(sect+"/type").toString();
        if (type.isEmpty())
        {
            qWarning() << QString("Section \"%1\" not found in %2.").arg(sect, mSettings->fileName());
            continue;
        }

        RazorPluginInfoList list = RazorPluginInfo::search(desktopDirs, "RazorPanel/Plugin", QString("%1.desktop").arg(type));
        if( !list.count())
        {
            qWarning() << QString("Plugin \"%1\" not found.").arg(type);
            continue;
        }

        loadPlugin(list.first(), sect);
    }
}


/************************************************

 ************************************************/
Plugin *RazorPanel::loadPlugin(const RazorPluginInfo &desktopFile, const QString &settingsGroup)
{
    Plugin *plugin = new Plugin(desktopFile, mSettings->fileName(), settingsGroup, this);
    if (plugin->isLoaded())
    {
        mPlugins.append(plugin);
        connect(plugin, SIGNAL(startMove()), mLayout, SLOT(startMovePlugin()));
        connect(plugin, SIGNAL(remove()), this, SLOT(removePlugin()));
        connect(this, SIGNAL(realigned()), plugin, SLOT(realign()));
        if (!plugin->isSeparate() && plugin->widget())
        {
            plugin->widget()->setFixedSize(QSize(mLineSize, mLineSize));
        }
        mLayout->addWidget(plugin);
        return plugin;
    }

    delete plugin;
    return 0;
}


/************************************************

 ************************************************/
void RazorPanel::realign()
{
    if (!isVisible())
        return;
#if 0
    qDebug() << "** Realign *********************";
    qDebug() << "LineSize:    " << mLineSize;
    qDebug() << "LineCount:   " << mLineCount;
    qDebug() << "Length:      " << mLength << (mLengthInPercents ? "%" : "px");
    qDebug() << "Alignment:   " << (mAlignment == 0 ? "center" : (mAlignment < 0 ? "left" : "right"));
    qDebug() << "Position:    " << positionToStr(mPosition) << "on" << mScreenNum;
    qDebug() << "Plugins count" << mPlugins.count();
#endif


    const QRect screen = QApplication::desktop()->screenGeometry(mScreenNum);
    QSize size = sizeHint();
    QRect rect;

    if (isHorizontal())
    {
        // Horiz panel ***************************

        // Size .......................
        rect.setHeight(qMax(PANEL_MINIMUM_SIZE, size.height()));
        if (mLengthInPercents)
            rect.setWidth(screen.width() * mLength / 100.0);
        else
            rect.setWidth(mLength);

        rect.setWidth(qMax(rect.size().width(), mLayout->minimumSize().width()));

        // Horiz ......................
        switch (mAlignment)
        {
        case RazorPanel::AlignmentLeft:
            rect.moveLeft(screen.left());
            break;

        case RazorPanel::AlignmentCenter:
            rect.moveCenter(screen.center());
            break;

        case RazorPanel::AlignmentRight:
            rect.moveRight(screen.right());
            break;
        }

        // Vert .......................
        if (mPosition == IRazorPanel::PositionTop)
            rect.moveTop(screen.top());
        else
            rect.moveBottom(screen.bottom());
    }
    else
    {
        // Vert panel ***************************

        // Size .......................
        rect.setWidth(qMax(PANEL_MINIMUM_SIZE, size.width()));
        if (mLengthInPercents)
            rect.setHeight(screen.height() * mLength / 100.0);
        else
            rect.setHeight(mLength);

        rect.setHeight(qMax(rect.size().height(), mLayout->minimumSize().height()));

        // Vert .......................
        switch (mAlignment)
        {
        case RazorPanel::AlignmentLeft:
            rect.moveTop(screen.top());
            break;

        case RazorPanel::AlignmentCenter:
            rect.moveCenter(screen.center());
            break;

        case RazorPanel::AlignmentRight:
            rect.moveBottom(screen.bottom());
            break;
        }

        // Horiz ......................
        if (mPosition == IRazorPanel::PositionLeft)
            rect.moveLeft(screen.left());
        else
            rect.moveRight(screen.right());
    }

    if (rect == geometry())
        return;

    setGeometry(rect);
    setFixedSize(rect.size());


    // Reserve our space on the screen ..........
    XfitMan xf = xfitMan();
    Window wid = effectiveWinId();
    QRect desktop = QApplication::desktop()->screen(mScreenNum)->geometry();

    switch (mPosition)
    {
        case RazorPanel::PositionTop:
            xf.setStrut(wid, 0, 0, height(), 0,
               /* Left   */  0, 0,
               /* Right  */  0, 0,
               /* Top    */  rect.left(), rect.right(),
               /* Bottom */  0, 0
                         );
        break;

        case RazorPanel::PositionBottom:
            xf.setStrut(wid, 0, 0, 0, desktop.height() - rect.y(),
               /* Left   */  0, 0,
               /* Right  */  0, 0,
               /* Top    */  0, 0,
               /* Bottom */  rect.left(), rect.right()
                         );
            break;

        case RazorPanel::PositionLeft:
            xf.setStrut(wid, width(), 0, 0, 0,
               /* Left   */  rect.top(), rect.bottom(),
               /* Right  */  0, 0,
               /* Top    */  0, 0,
               /* Bottom */  0, 0
                         );

            break;

        case RazorPanel::PositionRight:
            xf.setStrut(wid, 0, desktop.width() - rect.x(), 0, 0,
               /* Left   */  0, 0,
               /* Right  */  rect.top(), rect.bottom(),
               /* Top    */  0, 0,
               /* Bottom */  0, 0
                         );
            break;
    }
}


/************************************************
  The panel can't be placed on boundary of two displays.
  This function checks, is the panel can be placed on the display
  @displayNum on @position.
 ************************************************/
bool RazorPanel::canPlacedOn(int screenNum, RazorPanel::Position position)
{
    QDesktopWidget* dw = QApplication::desktop();

    switch (position)
    {
        case RazorPanel::PositionTop:
            for (int i=0; i < dw->screenCount(); ++i)
            {
                if (dw->screenGeometry(i).bottom() < dw->screenGeometry(screenNum).top())
                    return false;
            }
            return true;

        case RazorPanel::PositionBottom:
            for (int i=0; i < dw->screenCount(); ++i)
            {
                if (dw->screenGeometry(i).top() > dw->screenGeometry(screenNum).bottom())
                    return false;
            }
            return true;

        case RazorPanel::PositionLeft:
            for (int i=0; i < dw->screenCount(); ++i)
            {
                if (dw->screenGeometry(i).right() < dw->screenGeometry(screenNum).left())
                    return false;
            }
            return true;

        case RazorPanel::PositionRight:
            for (int i=0; i < dw->screenCount(); ++i)
            {
                if (dw->screenGeometry(i).left() > dw->screenGeometry(screenNum).right())
                    return false;
            }
            return true;
    }

    return false;
}


/************************************************

 ************************************************/
int RazorPanel::findAvailableScreen(RazorPanel::Position position)
{
    int current = mScreenNum;
    for (int i = current; i < QApplication::desktop()->screenCount(); ++i)
    {
        if (canPlacedOn(i, position))
            return i;
    }

    for (int i = 0; i < current; ++i)
    {
        if (canPlacedOn(i, position))
            return i;
    }

    return 0;
}


/************************************************

 ************************************************/
void RazorPanel::showConfigDialog()
{
    ConfigPanelDialog::exec(this);
}


/************************************************

 ************************************************/
void RazorPanel::showAddPluginDialog()
{
    AddPluginDialog* dialog = findChild<AddPluginDialog*>();

    if (!dialog)
    {
        dialog = new AddPluginDialog(pluginDesktopDirs(), "RazorPanel/Plugin", "*", this);
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        connect(dialog, SIGNAL(pluginSelected(const RazorPluginInfo&)), this, SLOT(addPlugin(const RazorPluginInfo&)));
    }
    
    RazorPluginInfoList pluginsInUse;
    foreach (Plugin *i, mPlugins)
        pluginsInUse << i->desktopFile();
    dialog->setPluginsInUse(pluginsInUse);

    dialog->show();
    dialog->raise();
    dialog->activateWindow();
    xfitMan().raiseWindow(dialog->effectiveWinId());
    xfitMan().moveWindowToDesktop(dialog->effectiveWinId(), qMax(xfitMan().getActiveDesktop(), 0));

    dialog->show();
}


/************************************************

 ************************************************/
void RazorPanel::addPlugin(const RazorPluginInfo &desktopFile)
{
    QString settingsGroup = findNewPluginSettingsGroup(desktopFile.id());
    loadPlugin(desktopFile, settingsGroup);
    emit realigned();
    saveSettings(true);
}


/************************************************

 ************************************************/
void RazorPanel::setLineSize(int value)
{
    if (mLineSize != value)
    {
        mLineSize = value;
        mLayout->setLineSize(mLineSize);
        foreach(Plugin *plugin, mPlugins)
        {
            if (!plugin->isSeparate() && plugin->widget())
            {
                plugin->widget()->setFixedSize(QSize(mLineSize, mLineSize));
            }
        }

        realign();
        emit realigned();
        saveSettings(true);
    }
}


/************************************************

 ************************************************/
void RazorPanel::setLineCount(int value)
{
    if (mLineCount != value)
    {
        mLineCount = value;
        mLayout->setEnabled(false);
        mLayout->setLineCount(mLineCount);
        realign();
        mLayout->setEnabled(true);
        emit realigned();
        saveSettings(true);
    }
}


/************************************************

 ************************************************/
void RazorPanel::setLength(int length, bool inPercents)
{
    if (mLength == length &&
        mLengthInPercents == inPercents)
        return;

    mLength = length;
    mLengthInPercents = inPercents;
    realign();
    saveSettings(true);
}


/************************************************

 ************************************************/
void RazorPanel::setPosition(int screen, IRazorPanel::Position position)
{
    if (mScreenNum == screen &&
        mPosition == position)
        return;

    mScreenNum = screen;
    mPosition = position;

    mLayout->setPosition(mPosition);

    realign();
    emit realigned();
    saveSettings(true);
}


/************************************************

 ************************************************/
void RazorPanel::setAlignment(RazorPanel::Alignment value)
{
    if (mAlignment == value)
        return;

    mAlignment = value;
    realign();
    saveSettings(true);
}


/************************************************

 ************************************************/
void RazorPanel::x11EventFilter(XEvent* event)
{
    QList<Plugin*>::iterator i;
    for (i = mPlugins.begin(); i != mPlugins.end(); ++i)
        (*i)->x11EventFilter(event);
}


/************************************************

 ************************************************/
QRect RazorPanel::globalGometry() const
{
    return QRect(mapToGlobal(QPoint(0, 0)), this->size());
}


/************************************************

 ************************************************/
bool RazorPanel::event(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::ContextMenu:
        showPopupMenu();
        break;

    case QEvent::LayoutRequest:
        realign();
        break;

    default:
        break;
    }
    return QFrame::event(event);
}


/************************************************

 ************************************************/
void RazorPanel::showEvent(QShowEvent *event)
{
    realign();
    emit realigned();
}


/************************************************

 ************************************************/
void RazorPanel::showPopupMenu(Plugin *plugin)
{
    QList<QMenu*> pluginsMenus;
    PopupMenu menu(tr("Panel"));

    menu.setIcon(XdgIcon::fromTheme("configure-toolbars"));

    // Plugin Menu ..............................
    if (plugin)
    {
        QMenu *m = plugin->popupMenu();

        if (m)
        {
            menu.addTitle(plugin->windowTitle().replace("&", "&&"));

            menu.addActions(m->actions());
            pluginsMenus << m;
        }
    }

    // Panel menu ...............................

    menu.addTitle(QIcon(), tr("Panel"));

    menu.addAction(tr("Configure panel..."),
                   this, SLOT(showConfigDialog())
                  );

    menu.addAction(XdgIcon::fromTheme("preferences-plugin"),
                   tr("Add plugins ..."),
                   this, SLOT(showAddPluginDialog())
                  );

    RazorPanelApplication *a = reinterpret_cast<RazorPanelApplication*>(qApp);
    menu.addAction(tr("Add panel"),
                   a, SLOT(addNewPanel())
                  );

    if (a->count() > 1)
    {
        menu.addAction(tr("Delete panel"),
                       this, SLOT(userRequestForDeletion())
                      );
    }

#ifdef DEBUG
    menu.addSeparator();
    menu.addAction("Exit (debug only)", qApp, SLOT(quit()));
#endif

    menu.exec(QCursor::pos());
    qDeleteAll(pluginsMenus);
}


/************************************************

 ************************************************/
Plugin *RazorPanel::findPlugin(const IRazorPanelPlugin *iPlugin) const
{
    foreach(Plugin *plugin, mPlugins)
    {
        if (plugin->iPlugin() == iPlugin)
            return plugin;
    }

    return 0;
}


/************************************************

 ************************************************/
QRect RazorPanel::calculatePopupWindowPos(const IRazorPanelPlugin *plugin, const QSize &windowSize) const
{
    Plugin *panelPlugin = findPlugin(plugin);
    if (!plugin)
        return QRect();

    int x=0, y=0;

    switch (position())
    {
        case IRazorPanel::PositionTop:
            x = panelPlugin->mapToGlobal(QPoint(0, 0)).x();
            y = globalGometry().bottom();
            break;

        case IRazorPanel::PositionBottom:
            x = panelPlugin->mapToGlobal(QPoint(0, 0)).x();
            y = globalGometry().top() - windowSize.height();
            break;

        case IRazorPanel::PositionLeft:
            x = globalGometry().right();
            y = panelPlugin->mapToGlobal(QPoint(0, 0)).y();
            break;

        case IRazorPanel::PositionRight:
            x = globalGometry().left() - windowSize.width();
            y = panelPlugin->mapToGlobal(QPoint(0, 0)).y();
            break;
    }

    QRect res(QPoint(x, y), windowSize);

    QRect screen = QApplication::desktop()->availableGeometry(this);

    if (res.right() > screen.right())
        res.moveRight(screen.right());

    if (res.bottom() > screen.bottom())
        res.moveBottom(screen.bottom());

    if (res.left() < screen.left())
        res.moveLeft(screen.left());

    if (res.top() < screen.top())
        res.moveTop(screen.top());

    return res;
}


/************************************************

 ************************************************/
QString RazorPanel::qssPosition() const
{
    return positionToStr(position());
}


/************************************************

 ************************************************/
QString RazorPanel::findNewPluginSettingsGroup(const QString &pluginType) const
{
    QStringList groups = mSettings->childGroups();
    groups.sort();

    // Generate new section name ................
    for (int i=2; true; ++i)
    {
        if (!groups.contains(QString("%1%2").arg(pluginType).arg(i)))
        {
            return QString("%1%2").arg(pluginType).arg(i);
        }
    }
}


/************************************************

 ************************************************/
void RazorPanel::removePlugin()
{
    Plugin *plugin = qobject_cast<Plugin*>(sender());
    if (plugin)
    {
        mPlugins.removeAll(plugin);
    }
    saveSettings();
}


/************************************************

 ************************************************/
void RazorPanel::pluginMoved()
{
    mPlugins.clear();
    for (int i=0; i<mLayout->count(); ++i)
    {
        Plugin *plugin = qobject_cast<Plugin*>(mLayout->itemAt(i)->widget());
        if (plugin)
            mPlugins << plugin;
    }
    saveSettings();
}

void RazorPanel::userRequestForDeletion()
{
    mSettings->beginGroup(mConfigGroup);
    QStringList plugins = mSettings->value("plugins").toStringList();
    mSettings->endGroup();
    
    Q_FOREACH(QString i, plugins)
        mSettings->remove(i);
    
    mSettings->remove(mConfigGroup);

    emit deletedByUser(this);
}
