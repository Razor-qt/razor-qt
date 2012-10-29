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
#include "razorpanel_p.h"
#include "razorpanellimits.h"
#include "irazorpanelplugin.h"
#include "razorpanelapplication.h"
#include "razorpanellayout.h"
#include "configpaneldialog.h"
#include "plugin.h"
#include <razorqt/addplugindialog/addplugindialog.h>
#include <razorqt/razorsettings.h>
#include <razorqt/razorplugininfo.h>
#include <razorqt/razorconfigdialog.h>

#include <QtCore/QDebug>
#include <QtCore/QLibrary>
#include <QtGui/QDesktopWidget>
#include <QtGui/QMenu>
#include <QtGui/QContextMenuEvent>
#include <QtCore/QSet>
#include <qtxdg/xdgicon.h>

#include <razorqt/xfitman.h>
#include <qtxdg/xdgdirs.h>
#include <QtGui/QWidgetAction>
#include <QtGui/QToolButton>



/************************************************
 Returns the Position by the string.
 String is one of "Top", "Left", "Bottom", "Right", string is not case sensitive.
 If the string is not correct, returns defaultValue.
 ************************************************/
IRazorPanel::Position RazorPanelPrivate::strToPosition(const QString& str, RazorPanel::Position defaultValue)
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
QString RazorPanelPrivate::positionToStr(IRazorPanel::Position position)
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
RazorPanel::RazorPanel(QWidget *parent) :
  QFrame(parent),
  d_ptr(new RazorPanelPrivate(this))
{
    // Read command line arguments ..............
    // The first argument is config file name.
    QString configFile = "panel";
    if (qApp->arguments().count() > 1)
    {
        configFile = qApp->arguments().at(1);
        if (configFile.endsWith(".conf"))
            configFile.chop(5);
    }


    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_X11NetWmWindowTypeDock);
    setAttribute(Qt::WA_AlwaysShowToolTips);
    setWindowTitle("Razor Panel");
    setObjectName("RazorPanel");

    Q_D(RazorPanel);

    d->mLayout = new RazorPanelLayout(QBoxLayout::LeftToRight, parent);
    connect(d->mLayout, SIGNAL(widgetMoved(QWidget*)), d, SLOT(pluginMoved(QWidget*)));
    setLayout(d->mLayout);

    connect(QApplication::desktop(), SIGNAL(resized(int)), d_ptr, SLOT(screensChangeds()));
    connect(QApplication::desktop(), SIGNAL(screenCountChanged(int)), d_ptr, SLOT(screensChangeds()));
    connect(RazorSettings::globalSettings(), SIGNAL(settingsChanged()), this, SLOT(update()));
    connect(razorApp, SIGNAL(themeChanged()), this, SLOT(realign()));

    d->mSettings = new RazorSettings("razor-panel/" + configFile, this);
    d->readSettings();

    d->loadPlugins();
    d->reposition();

}

/************************************************

 ************************************************/
void RazorPanelPrivate::readSettings()
{
    // Read settings ............................
    mSettings->beginGroup(CFG_PANEL_GROUP);
    mPosition = strToPosition(mSettings->value(CFG_KEY_POSITION).toString(), RazorPanel::PositionBottom);
    mScreenNum = mSettings->value(CFG_KEY_SCREENNUM, QApplication::desktop()->primaryScreen()).toInt();
    mHeight = mSettings->value(CFG_KEY_HEIGHT, PANEL_DEFAULT_SIZE).toInt();
    mAlignment = RazorPanel::Alignment(mSettings->value(CFG_KEY_ALIGNMENT, 0).toInt());
    mWidthInPercents = mSettings->value(CFG_KEY_PERCENT, true).toBool();
    mWidth = mSettings->value(CFG_KEY_WIDTH, 100).toInt();
    mUseThemeSize = mSettings->value(CFG_KEY_AUTOSIZE, true).toBool();
    mSettings->endGroup();
}


/************************************************

 ************************************************/
RazorPanelPrivate::RazorPanelPrivate(RazorPanel* parent):
    QObject(parent),
    q_ptr(parent)
{
    mScreenNum = 0;
    mSpacer = 0;
}


/************************************************

 ************************************************/
void RazorPanelPrivate::screensChangeds()
{
    if (! canPlacedOn(mScreenNum, mPosition))
        mScreenNum = findAvailableScreen(mPosition);

    reposition();
}


/************************************************

 ************************************************/
RazorPanel::~RazorPanel()
{
    Q_D(RazorPanel);
    d->saveSettings();
    qDeleteAll(d->mPlugins);
    d->mSettings->sync();
    delete d;
}


/************************************************

 ************************************************/
void RazorPanelPrivate::saveSettings()
{
    qDebug() << Q_FUNC_INFO;
    QStringList pluginsStr;

    for (int i=0; i<mLayout->count(); ++i)
    {
        qDebug() << "::" << i;
        Plugin* plugin = qobject_cast<Plugin*>(mLayout->itemAt(i)->widget());
        if (plugin)
        {
            plugin->saveSettings();
            pluginsStr << plugin->settingsGroup();
            qDebug() << " * " << pluginsStr;
        }
    }

    mSettings->beginGroup(CFG_PANEL_GROUP);
    mSettings->setValue(CFG_KEY_PLUGINS, pluginsStr);
    mSettings->endGroup();
    mSettings->sync();
}


/************************************************

 ************************************************/
QStringList RazorPanelPrivate::pluginDesktopDirs()
{
    QStringList dirs;
    dirs << QString(getenv("RAZORQT_PANEL_PLUGINS_DIR")).split(':', QString::SkipEmptyParts);
    dirs << QString("%1/%2").arg(XdgDirs::dataHome(), "/razor/razor-panel");
    dirs << PLUGIN_DESKTOPS_DIR;
    return dirs;
}


/************************************************

 ************************************************/
void RazorPanelPrivate::loadPlugins()
{
    QStringList desktopDirs = pluginDesktopDirs();
    QStringList sections = mSettings->value(CFG_FULLKEY_PLUGINS).toStringList();

    foreach (QString sect, sections)
    {
        //qDebug() << "** Load plugin" << sect << "*************";
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


    QList<Plugin*>::iterator i;

    // Add left plugins .........................
    for (i=mPlugins.begin(); i != mPlugins.end(); ++i)
    {
        if ((*i)->alignment() != Plugin::AlignLeft)
            break;

        mLayout->addWidget((*i));
    }

    mLayout->addStretch();
    mSpacer = mLayout->itemAt(mLayout->count() -1);

    // Add right plugins ........................
    for (; i != mPlugins.end(); ++i)
    {
        mLayout->addWidget((*i));
    }

}


/************************************************

 ************************************************/
Plugin *RazorPanelPrivate::loadPlugin(const RazorPluginInfo &desktopFile, const QString &settingsGroup)
{
    Q_Q(RazorPanel);
    Plugin *plugin = new Plugin(desktopFile, mSettings->fileName(), settingsGroup, q);
    if (plugin->isLoaded())
    {
        mPlugins.append(plugin);
        connect(plugin, SIGNAL(move(QWidget*)), mLayout, SLOT(startMoveWidget(QWidget*)));
        connect(plugin, SIGNAL(remove()), this, SLOT(pluginRemoved()));
        return plugin;
    }

    return 0;
}


/************************************************

 ************************************************/
void RazorPanelPrivate::realign()
{
    Q_Q(RazorPanel);
    /*
    qDebug() << "Realign: DesktopNum" << mScreenNum;
    qDebug() << "Realign: Position  " << positionToStr(mPosition);
    //qDebug() << "Realign: Theme     " << mTheme;
    qDebug() << "Realign: SizeHint  " << q->sizeHint();
    qDebug() << "Realign: Screen    " << QApplication::desktop()->screenGeometry(mScreenNum);
    */

    // Update stylesheet ............
    q->style()->unpolish(q);
    q->style()->polish(q);
    // ..............................

    QRect screen = QApplication::desktop()->screenGeometry(mScreenNum);
    QRect rect;

    if (q->isHorizontal())
    {
        // Size .....................
        rect.setHeight(mHeight);
        if (mWidthInPercents)
            rect.setWidth(screen.width() * mWidth / 100);
        else
            rect.setWidth(mWidth);        

        q->setGeometry(rect);
        rect = q->geometry();
        // Horiz ....................
        switch (mAlignment)
        {
        case RazorPanel::AlignmentLeft:
            rect.moveLeft(screen.left());
            break;
        case RazorPanel::AlignmentCenter:
            rect.moveCenter(QPoint(screen.center()));
            break;
        case RazorPanel::AlignmentRight:
            rect.moveRight(screen.right());
            break;
        }

        // Vert .....................
        if (mPosition == RazorPanel::PositionTop)
            rect.moveTop(screen.top());
        else
        {
            rect.moveBottom(screen.bottom());
        }
    }
    else
    {
        // Size .....................
        rect.setWidth(mHeight);
        if (mWidthInPercents)
            rect.setHeight(screen.height() * mWidth / 100);
        else
            rect.setHeight(mWidth);

        q->setGeometry(rect);
        rect = q->geometry();

        // Vert .....................
        switch (mAlignment)
        {
        case RazorPanel::AlignmentLeft:
            rect.moveTop(screen.top());
            break;
        case RazorPanel::AlignmentCenter:
            rect.moveCenter(QPoint(screen.center()));
            break;
        case RazorPanel::AlignmentRight:
            rect.moveBottom(screen.bottom());
            break;
        }

        // Horiz ....................
        if (mPosition == RazorPanel::PositionLeft)
            rect.moveLeft(screen.left());
        else
            rect.moveRight(screen.right());
    }
    q->setGeometry(rect);

    // Reserve our space on the screen ..........
    XfitMan xf = xfitMan();
    Window wid = q->effectiveWinId();
    QRect desktop = QApplication::desktop()->screen(mScreenNum)->geometry();

    switch (mPosition)
    {
        case RazorPanel::PositionTop:
            xf.setStrut(wid, 0, 0, q->height(), 0,
               /* Left   */   0, 0,
               /* Right  */   0, 0,
               /* Top    */   rect.left(), rect.right(),
               /* Bottom */   0, 0
                         );
        break;

        case RazorPanel::PositionBottom:
            xf.setStrut(wid,  0, 0, 0, desktop.height() - rect.y(),
               /* Left   */   0, 0,
               /* Right  */   0, 0,
               /* Top    */   0, 0,
               /* Bottom */   rect.left(), rect.right()
                         );
            break;

        case RazorPanel::PositionLeft:
            xf.setStrut(wid, q->width(), 0, 0, 0,
               /* Left   */   rect.top(), rect.bottom(),
               /* Right  */   0, 0,
               /* Top    */   0, 0,
               /* Bottom */   0, 0
                         );

            break;

        case RazorPanel::PositionRight:
            xf.setStrut(wid, 0, desktop.width() - rect.x(), 0, 0,
               /* Left   */   0, 0,
               /* Right  */   rect.top(), rect.bottom(),
               /* Top    */   0, 0,
               /* Bottom */   0, 0
                         );
            break;
    }

    emit q->panelRealigned();
}


/************************************************
  The panel can't be placed on boundary of two displays.
  This function checks, is the panel can be placed on the display
  @displayNum on @position.
 ************************************************/
bool RazorPanelPrivate::canPlacedOn(int screenNum, RazorPanel::Position position)
{
    QDesktopWidget* dw = QApplication::desktop();

    switch (position)
    {
        case RazorPanel::PositionTop:
            for (int i=0; i < dw->screenCount(); ++i)
                if (dw->screenGeometry(i).bottom() < dw->screenGeometry(screenNum).top())
                    return false;
                return true;

        case RazorPanel::PositionBottom:
            for (int i=0; i < dw->screenCount(); ++i)
                if (dw->screenGeometry(i).top() > dw->screenGeometry(screenNum).bottom())
                    return false;
                return true;

        case RazorPanel::PositionLeft:
            for (int i=0; i < dw->screenCount(); ++i)
                if (dw->screenGeometry(i).right() < dw->screenGeometry(screenNum).left())
                    return false;
                return true;
//            return false;

        case RazorPanel::PositionRight:
            for (int i=0; i < dw->screenCount(); ++i)
                if (dw->screenGeometry(i).left() > dw->screenGeometry(screenNum).right())
                    return false;
                return true;
//            return false;
    }
    return false;
}


/************************************************

 ************************************************/
int RazorPanelPrivate::findAvailableScreen(RazorPanel::Position position)
{
    for (int i = mScreenNum; i < QApplication::desktop()->screenCount(); ++i)
    {
        if (canPlacedOn(i, position))
            return i;
    }

    for (int i = 0; i < mScreenNum; ++i)
    {
        if (canPlacedOn(i, position))
            return i;
    }

    return 0;
}


/************************************************

 ************************************************/
void RazorPanelPrivate::pluginMoved(QWidget* pluginWidget)
{
    Plugin* plugin = qobject_cast<Plugin*>(pluginWidget);
    if (!plugin)
        return;

    for (int i=0; i<mLayout->count(); ++i)
    {
        if (mLayout->itemAt(i) == mSpacer)
        {
            plugin->setAlignment(Plugin::AlignRight);
            break;
        }

        if (mLayout->itemAt(i)->widget() == plugin)
        {
            plugin->setAlignment(Plugin::AlignLeft);
            break;
        }
    }
    saveSettings();
}


/************************************************

 ************************************************/
void RazorPanelPrivate::showAddPluginDialog()
{
    Q_Q(RazorPanel);
    AddPluginDialog* dlg = q->findChild<AddPluginDialog*>();

    if (!dlg)
    {
        dlg = new AddPluginDialog(pluginDesktopDirs(), "RazorPanel/Plugin", "*", q);
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        connect(dlg, SIGNAL(pluginSelected(const RazorPluginInfo&)), this, SLOT(addPlugin(const RazorPluginInfo&)));
    }

    dlg->show();

}

/************************************************

 ************************************************/
void RazorPanelPrivate::showConfigPanelDialog()
{
    Q_Q(RazorPanel);
    QRect screen = QApplication::desktop()->screenGeometry(mScreenNum);
    RazorConfigDialog* dlg = new RazorConfigDialog(tr("Configure panel"), mSettings, q);
    ConfigPanelDialog* page = new ConfigPanelDialog (PANEL_DEFAULT_SIZE, screen.width(), mSettings, dlg);
    page->setSizeLimits(PANEL_MINIMUM_SIZE, PANEL_MAXIMUM_SIZE);
    dlg->addPage(page, tr("Configure panel"));

    dlg->setAttribute(Qt::WA_DeleteOnClose);
    connect(dlg, SIGNAL(reset()), page, SLOT(reset()));
    connect(dlg, SIGNAL(save()), page, SLOT(save()));
    connect(page, SIGNAL(configChanged(int, int, bool, RazorPanel::Alignment, bool)),
                      SLOT(updateSize(int, int, bool, RazorPanel::Alignment, bool)));
    connect(page, SIGNAL(positionChanged(int, RazorPanel::Position)), SLOT(switchPosition(int, RazorPanel::Position)));
    dlg->show();
}


/************************************************

 ************************************************/
void RazorPanelPrivate::updateSize(int height, int width, bool percent, RazorPanel::Alignment alignment, bool useThemeSize)
{
    mHeight = height;
    mWidth = width;
    mWidthInPercents = percent;
    mAlignment = alignment;    
    mUseThemeSize = useThemeSize;
    realign();
}


/************************************************

 ************************************************/
QString RazorPanelPrivate::findNewPluginSettingsGroup(const QString &pluginType) const
{
    QSet<QString> loadedPlugins = QSet<QString>::fromList(mSettings->value(CFG_FULLKEY_PLUGINS).toStringList());

    QStringList groups = mSettings->childGroups();
    groups.sort();

    // Search free section name .................
    foreach(const QString &group, groups)
    {
        if (!loadedPlugins.contains(group))
        {
            if (mSettings->value(group + "/type") == pluginType)
            {
                return group;
            }
        }
    }

    // Generate new section name ................
    for (int i=2; true; ++i)
    {
        if (!groups.contains(QString("%1%2").arg(pluginType).arg(i)))
            return QString("%1%2").arg(pluginType).arg(i);
    }
}


/************************************************

 ************************************************/
void RazorPanelPrivate::addPlugin(const RazorPluginInfo &desktopFile)
{
    //Q_Q(RazorPanel);

    QString settingsGroup = findNewPluginSettingsGroup(desktopFile.id());
    Plugin *plugin= loadPlugin(desktopFile, settingsGroup);

    if (!plugin)
        return;

    if (plugin->alignment() == Plugin::AlignLeft)
        mLayout->insertWidget(0, plugin);
    else
        mLayout->addWidget(plugin);

//    realign();
//    q->adjustSize();
    saveSettings();
}


/************************************************

 ************************************************/
void RazorPanel::show()
{
    Q_D(RazorPanel);
    QWidget::show();
    d->reposition();
    xfitMan().moveWindowToDesktop(this->effectiveWinId(), -1);
}


/************************************************

 ************************************************/
RazorPanel::Position RazorPanel::position() const
{
    Q_D(const RazorPanel);
    return d->mPosition;
}


/************************************************

 ************************************************/
void RazorPanel::x11EventFilter(XEvent* event)
{
    Q_D(RazorPanel);
    foreach (Plugin* plugin, d->mPlugins)
    {
        plugin->x11EventFilter(event);
    }
}


/************************************************

 ************************************************/
QRect RazorPanel::globalGometry() const
{
    return QRect(mapToGlobal(QPoint(0, 0)), this->size());
}


/************************************************

 ************************************************/
bool RazorPanel::event(QEvent* e)
{
    Q_D(RazorPanel);
    switch (e->type())
    {
        case QEvent::ContextMenu:
            showPopupMenu();
            break;

        case QEvent::Resize:
            d->realign();
            break;

        default:
            break;
    }
    return QFrame::event(e);
}


/************************************************

 ************************************************/
void RazorPanel::showPopupMenu(Plugin *plugin)
{
    Q_D(RazorPanel);

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
                   d, SLOT(showConfigPanelDialog())
                  );

    menu.addAction(XdgIcon::fromTheme("preferences-plugin"),
                   tr("Add plugins ..."),
                   d, SLOT(showAddPluginDialog())
                  );

    // Annoying sub-menu:
//    QMenu* pluginsMenu = menu.addMenu(tr("Plugins"));
//    foreach (RazorPanelPlugin* p, d->mPlugins)
//    {
//        QMenu *m = p->popupMenu();
//        if (m)
//        {
//            pluginsMenu->addMenu(m);
//            pluginsMenus.append(m);
//        }
//    }

#ifdef DEBUG
    menu.addSeparator();
    menu.addAction("Exit (debug only)", this, SLOT(close()));
#endif

    menu.exec(QCursor::pos());
    qDeleteAll(pluginsMenus);
}


/************************************************

 ************************************************/
void RazorPanelPrivate::pluginRemoved()
{
    Plugin *p = qobject_cast<Plugin*>(sender());
    if (p)
    {
        mLayout->removeWidget(p);
        mPlugins.removeAll(p);
        saveSettings();
    }
}


/************************************************

 ************************************************/
void RazorPanelPrivate::switchPosition(int screenNum, RazorPanel::Position position)
{
    mPosition = position;
    mScreenNum = screenNum;
    reposition();
}

/************************************************

 ************************************************/
void RazorPanelPrivate::reposition()
{
    Q_Q(RazorPanel);
/*
    QRect screen = QApplication::desktop()->screenGeometry(mScreenNum);

    if (q->isHorizontal())
        mLayout->setDirection(QBoxLayout::LeftToRight);
    else
        mLayout->setDirection(QBoxLayout::TopToBottom);

    foreach (Plugin* plugin, mPlugins)
    {
        plugin->updateSizePolicy();
    }

    emit q->layoutDirectionChanged(q->isHorizontal() ? QBoxLayout::LeftToRight : QBoxLayout::TopToBottom);
    emit q->positionChanged();

    if (q->isHorizontal())
    {
        if (!mUseThemeSize)
        {
            q->setMaximumSize(screen.width(), mHeight);
        }
        else
        {
            q->setMaximumWidth(screen.width());
        }
    }
    else
    {
        if (!mUseThemeSize)
        {
            q->setMaximumSize(mHeight, screen.height());
        }
        else
        {
            q->setMaximumHeight(screen.height());
        }
    }
*/
    realign();
}


static const char POPUPMENU_TITLE[] = "POPUP_MENU_TITLE_OBJECT_NAME";

/************************************************

 ************************************************/
QAction* PopupMenu::addTitle(const QIcon &icon, const QString &text)
{
    QAction *buttonAction = new QAction(this);
    QFont font = buttonAction->font();
    font.setBold(true);
    buttonAction->setText(QString(text).replace("&", "&&"));
    buttonAction->setFont(font);
    buttonAction->setIcon(icon);

    QWidgetAction *action = new QWidgetAction(this);
    action->setObjectName(POPUPMENU_TITLE);
    QToolButton *titleButton = new QToolButton(this);
    titleButton->installEventFilter(this); // prevent clicks on the title of the menu
    titleButton->setDefaultAction(buttonAction);
    titleButton->setDown(true); // prevent hover style changes in some styles
    titleButton->setCheckable(true);
    titleButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    action->setDefaultWidget(titleButton);

    addAction(action);
    return action;
}


/************************************************

 ************************************************/
QAction* PopupMenu::addTitle(const QString &text)
{
    return addTitle(QIcon(), text);
}


/************************************************

 ************************************************/
void PopupMenu::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Up || e->key() == Qt::Key_Down)
    {
        QMenu::keyPressEvent(e);

        QWidgetAction *action = qobject_cast<QWidgetAction*>(this->activeAction());
        QWidgetAction *firstAction = action;

        while (action && action->objectName() == POPUPMENU_TITLE)
        {
            this->keyPressEvent(e);
            action = qobject_cast<QWidgetAction*>(this->activeAction());

            if (firstAction == action) // we looped and only found titles
            {
                this->setActiveAction(0);
                break;
            }
        }

        return;
    }

    QMenu::keyPressEvent(e);
}


/************************************************

 ************************************************/
Plugin *RazorPanelPrivate::findPlugin(const IRazorPanelPlugin *iPlugin) const
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
    Q_D(const RazorPanel);

    Plugin *panelPlugin = d->findPlugin(plugin);
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

    return QRect(QPoint(x, y), windowSize);
}

QString RazorPanel::qssPosition() const
{
    Q_D(const RazorPanel);
    return d->positionToStr(d->mPosition);
}


