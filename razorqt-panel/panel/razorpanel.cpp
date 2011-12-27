/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL3+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.ru>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
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
#include "razorpanelplugin.h"
#include "razorpanelapplication.h"
#include "razorpanellayout.h"
#include "configpaneldialog.h"
#include <razorqt/addplugindialog/addplugindialog.h>
#include <razorqt/razorsettings.h>
#include <razorqt/razorplugininfo.h>

#include <QtCore/QString>
#include <QtCore/QDebug>
#include <QtCore/QLocale>
#include <QtCore/QTranslator>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtCore/QRect>
#include <QtGui/QMenu>
#include <QtGui/QContextMenuEvent>
#include <QtCore/QFile>
#include <QtGui/QAction>
#include <QtGui/QActionGroup>
#include <QtGui/QStyle>
#include <QtCore/QFileInfo>
#include <QtCore/QLibrary>
#include <QtGui/QSpacerItem>

#include <qtxdg/xdgicon.h>
#include <razorqt/xfitman.h>


#define CFG_PANEL_GROUP     "panel"

#define CFG_KEY_SCREENNUM   "desktop"
#define CFG_KEY_POSITION    "position"
#define CFG_KEY_PLUGINS     "plugins"
#define CFG_KEY_HEIGHT      "height"
#define CFG_KEY_WIDTH       "width"
#define CFG_KEY_WIDTH_TYPE  "widthType"
#define CFG_KEY_ALIGNMENT   "alignment"


#define CFG_FULLKEY_PLUGINS "panel/plugins"



/************************************************
 Returns the Position by the string.
 String is one of "Top", "Left", "Bottom", "Right", string is not case sensitive.
 If the string is not correct, returns defaultValue.
 ************************************************/
RazorPanel::Position strToPosition(const QString& str, RazorPanel::Position defaultValue)
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
QString positionToStr(RazorPanel::Position position)
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
    Q_D(RazorPanel);
    d->init();

    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_X11NetWmWindowTypeDock);
    setAttribute(Qt::WA_AlwaysShowToolTips);

    setObjectName("RazorPanel");
}


/************************************************

 ************************************************/
RazorPanelPrivate::RazorPanelPrivate(RazorPanel* parent):
    QObject(parent),
    q_ptr(parent),
    mScreenNum(0)
{
    // Read command line arguments ..............
    // The first argument is config file name.
    mConfigFile = "panel";
    if (qApp->arguments().count() > 1)
    {
        mConfigFile = qApp->arguments().at(1);
        if (mConfigFile.endsWith(".conf"))
            mConfigFile.chop(5);
    }

    mSettings = new RazorSettings("razor-panel/" + mConfigFile, this);

    mLayout = new RazorPanelLayout(QBoxLayout::LeftToRight, parent);
    connect(mLayout, SIGNAL(widgetMoved(QWidget*)), this, SLOT(pluginMoved(QWidget*)));

    connect(QApplication::desktop(), SIGNAL(resized(int)), this, SLOT(screensChangeds()));
    connect(QApplication::desktop(), SIGNAL(screenCountChanged(int)), this, SLOT(screensChangeds()));
    //connect(QApplication::desktop(), SIGNAL(workAreaResized(int)), this, SLOT(screensChangeds()));

    XdgIcon::setThemeName(RazorSettings::globalSettings()->value("icon_theme").toString());
    connect(RazorSettings::globalSettings(), SIGNAL(settigsChanged()), q_ptr, SLOT(update()));
}


/************************************************

 ************************************************/
void RazorPanelPrivate::screensChangeds()
{
    if (! canPlacedOn(mScreenNum, mPosition))
        mScreenNum = findAvailableScreen(mPosition);

    realign();
}


/************************************************

 ************************************************/
void RazorPanelPrivate::init()
{
    Q_Q(RazorPanel);
    // Read settings ............................
    mSettings->beginGroup(CFG_PANEL_GROUP);
    mPosition = strToPosition(mSettings->value(CFG_KEY_POSITION).toString(), RazorPanel::PositionBottom);
    mScreenNum = mSettings->value(CFG_KEY_SCREENNUM, QApplication::desktop()->primaryScreen()).toInt();
    mSettings->endGroup();

    q->setLayout(mLayout);

    loadPlugins();
    reTheme();
}



/************************************************

 ************************************************/
RazorPanel::~RazorPanel()
{
    Q_D(RazorPanel);
    d->saveSettings();
    delete d;
}


/************************************************

 ************************************************/
RazorPanelPrivate::~RazorPanelPrivate()
{
    qDeleteAll(mPlugins);
    mSettings->sync();
}


/************************************************

 ************************************************/
void RazorPanelPrivate::saveSettings()
{
    foreach (RazorPanelPlugin* plugin, mPlugins)
    {
        if (plugin->alignment() == RazorPanelPlugin::AlignLeft)
            plugin->settings().setValue("alignment", "Left");
        else
            plugin->settings().setValue("alignment", "Right");
    }

    mSettings->beginGroup(CFG_PANEL_GROUP);
    mSettings->setValue(CFG_KEY_SCREENNUM, mScreenNum);
    mSettings->setValue(CFG_KEY_POSITION, positionToStr(mPosition));

    QStringList pluginsStr;
    for (int i=0; i<mLayout->count(); ++i)
    {
        RazorPanelPlugin* plugin = qobject_cast<RazorPanelPlugin*>(mLayout->itemAt(i)->widget());
        if (plugin)
            pluginsStr << plugin->configId();
    }

    mSettings->setValue(CFG_KEY_PLUGINS, pluginsStr);
    mSettings->endGroup();
    mSettings->sync();
}


/************************************************

 ************************************************/
void RazorPanelPrivate::loadPlugins()
{
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

        RazorPluginInfo pi;
        pi.load(QString("%1/%2.desktop").arg(PLUGIN_DESKTOPS_DIR, type));

        if (pi.isValid() && pi.serviceType() == "RazorPanel/Plugin")
            loadPlugin(&pi, sect);
        else
            qWarning() << QString("Plugin \"%1\" not found.").arg(type);
    }


    QList<RazorPanelPlugin*>::iterator i;
    // Add left plugins .........................
    for (i=mPlugins.begin(); i != mPlugins.end(); ++i)
    {
        if ((*i)->alignment() != RazorPanelPlugin::AlignLeft)
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
RazorPanelPlugin* RazorPanelPrivate::loadPlugin(const RazorPluginInfo* pluginInfo, const QString configSection)
{
    Q_Q(RazorPanel);

    QLibrary* lib = pluginInfo->loadLibrary(PLUGIN_DIR);
    if (!lib)
        return 0;

    PluginInitFunction initFunc = (PluginInitFunction) lib->resolve("init");

    if (!initFunc)
        return 0;

    RazorPanelPluginStartInfo startInfo(mSettings, configSection, q, pluginInfo);
    //startInfo.configFile = mSettings->fileName();
    //startInfo.configSection = configSection;
    //startInfo.panel = q;
    //startInfo.pluginInfo = pluginInfo;
    RazorPanelPlugin* plugin = initFunc(&startInfo, q);
    if (!plugin)
        return 0;

    connect(q, SIGNAL(layoutDirectionChanged(QBoxLayout::Direction)),
            plugin, SLOT(layoutDirectionChanged(QBoxLayout::Direction)));

    mPlugins.append(plugin);
    return plugin;
}


/************************************************

 ************************************************/
void RazorPanelPrivate::reTheme()
{
    qApp->setStyleSheet(razorTheme->qss("panel"));
    realign();
}



/************************************************

 ************************************************/
void RazorPanelPrivate::realign()
{
    Q_Q(RazorPanel);
    /*
    qDebug() << "Realign: DesktopNum" << mScreenNum;
    qDebug() << "Realign: Position  " << positionToStr(mPosition);
    qDebug() << "Realign: Theme     " << mTheme;
    qDebug() << "Realign: SizeHint  " << q->sizeHint();
    qDebug() << "Realign: Screen    " << QApplication::desktop()->screenGeometry(mScreenNum);
    */

    // Update stylesheet ............
    q->style()->unpolish(q);
    q->style()->polish(q);
    // ..............................

    if (q->isHorizontal()) mLayout->setDirection(QBoxLayout::LeftToRight);
    else  mLayout->setDirection(QBoxLayout::TopToBottom);

    QRect screen = QApplication::desktop()->screenGeometry(mScreenNum);
    QRect rect = screen;
    QSize sizeHint = q->sizeHint();

    // Panel height: load from file, else - use default
    mSettings->beginGroup(CFG_PANEL_GROUP);
    int mHeight = mSettings->value(CFG_KEY_HEIGHT, sizeHint.height()).toInt();
    int mWidthType = mSettings->value(CFG_KEY_WIDTH_TYPE, 0).toInt();
    int mWidth = mSettings->value(CFG_KEY_WIDTH, 100).toInt();
    int mAlignment = mSettings->value(CFG_KEY_ALIGNMENT, 0).toInt();
    mSettings->endGroup();

    switch (mPosition)
    {
        case RazorPanel::PositionTop:
        rect.setHeight(mHeight);

        if (mWidthType==0)   // size in percents
        {
            if (mAlignment==2)      //align - center
                rect.setLeft((screen.width()-(screen.width()*mWidth/100))/2);
            if (mAlignment==1) //align - rigth
                rect.setLeft(screen.width()-(screen.width()*mWidth/100));
            rect.setWidth(screen.width()*mWidth/100);
        }
        else                // size in pixels
        {
            if (mAlignment==2)      //align - center
                rect.setLeft((screen.width()-mWidth)/2);
            if (mAlignment==1) //align - rigth
                rect.setLeft(screen.width()-mWidth);
            rect.setWidth(mWidth);
        }
            break;

        case RazorPanel::PositionBottom:
            rect.setHeight(mHeight);

            if (mWidthType==0)   // size in percents
            {
                if (mAlignment==2)      //align - center
                    rect.setLeft((screen.width()-(screen.width()*mWidth/100))/2);
                if (mAlignment==1) //align - rigth
                    rect.setLeft(screen.width()-(screen.width()*mWidth/100));
                rect.setWidth(screen.width()*mWidth/100);
            }
            else                // size in pixels
            {
                if (mAlignment==2)      //align - center
                    rect.setLeft((screen.width()-mWidth)/2);
                if (mAlignment==1) //align - rigth
                    rect.setLeft(screen.width()-mWidth);
                rect.setWidth(mWidth);
            }
            rect.moveBottom(screen.bottom());
            break;

        case RazorPanel::PositionLeft:
            rect.setWidth(sizeHint.width());
            break;

        case RazorPanel::PositionRight:
            rect.setWidth(sizeHint.width());
            rect.moveRight(screen.right());
            break;
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

}


/************************************************
  The panel can't be placed on boundary of two displays.
  This function checks, is the panel can be placed on the display
  @displayNum on @position.
 ************************************************/
bool RazorPanelPrivate::canPlacedOn(int screenNum, RazorPanel::Position position) const
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
//            for (int i=0; i < dw->screenCount(); ++i)
//                if (dw->screenGeometry(i).right() < dw->screenGeometry(screenNum).left())
//                    return false;
//            return true;
            return false;

        case RazorPanel::PositionRight:
//            for (int i=0; i < dw->screenCount(); ++i)
//                if (dw->screenGeometry(i).left() > dw->screenGeometry(screenNum).right())
//                    return false;
//            return true;
            return false;
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
PositionAction::PositionAction(int displayNum, RazorPanel::Position position, QActionGroup *parent):
    QAction(parent)
{
    if (QApplication::desktop()->screenCount() == 1)
    {
        switch (position)
        {
            case RazorPanel::PositionTop:    setText(tr("Top of desktop"));      break;
            case RazorPanel::PositionBottom: setText(tr("Bottom of desktop"));   break;
            case RazorPanel::PositionLeft:   setText(tr("Left of desktop"));     break;
            case RazorPanel::PositionRight:  setText(tr("Right of desktop"));    break;
        }
    }
    else
    {
        switch (position)
        {
        case RazorPanel::PositionTop:    setText(tr("Top of desktop %1").arg(displayNum +1));    break;
        case RazorPanel::PositionBottom: setText(tr("Bottom of desktop %1").arg(displayNum +1)); break;
        case RazorPanel::PositionLeft:   setText(tr("Left of desktop %1").arg(displayNum +1));   break;
        case RazorPanel::PositionRight:  setText(tr("Right of desktop %1").arg(displayNum +1));  break;
        }
    }


    mPosition = position;
    mDisplayNum = displayNum;
    setCheckable(true);
    parent->addAction(this);
}


/************************************************

 ************************************************/
void RazorPanelPrivate::pluginMoved(QWidget* pluginWidget)
{
    RazorPanelPlugin* plugin = qobject_cast<RazorPanelPlugin*>(pluginWidget);
    if (!plugin)
        return;

    for (int i=0; i<mLayout->count(); ++i)
    {
        if (mLayout->itemAt(i) == mSpacer)
        {
            plugin->setAlignment(RazorPanelPlugin::AlignRight);
            break;
        }

        if (mLayout->itemAt(i)->widget() == plugin)
        {
            plugin->setAlignment(RazorPanelPlugin::AlignLeft);
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
        dlg = new AddPluginDialog(PLUGIN_DESKTOPS_DIR, "RazorPanel/Plugin", "*", q);
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
    QSize sizeHint = q->sizeHint();

    int heightDefault=sizeHint.height();
    int widthMax=screen.width();
    ConfigPanelDialog* dlg = new ConfigPanelDialog (heightDefault, widthMax, q);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    //dlg->exec();
    dlg->show();

}

/************************************************

 ************************************************/
void RazorPanelPrivate::addPlugin(const RazorPluginInfo &pluginInfo)
{
    QString sectionName = pluginInfo.id();
    QStringList groups = mSettings->childGroups();

    if (groups.contains(sectionName))
    {
        for (int i=2; true; ++i)
        {
            sectionName = QString("%1%2").arg(pluginInfo.id()).arg(i);
            if (!groups.contains(sectionName))
                break;
        }
    }

    RazorPanelPlugin* plugin = loadPlugin(&pluginInfo, sectionName);
    if (!plugin)
        return;

    mSettings->setValue(QString("%1/type").arg(sectionName), pluginInfo.id());

    if (plugin->flags().testFlag(RazorPanelPlugin::PreferRightAlignment))
        mLayout->addWidget(plugin);
    else
        mLayout->insertWidget(0, plugin);

    saveSettings();
}


/************************************************

 ************************************************/
void RazorPanel::show()
{
    Q_D(RazorPanel);
    QWidget::show();
    d->realign();
    xfitMan().moveWindowToDesktop(this->effectiveWinId(), -1);
}


/************************************************

 ************************************************/
RazorPanel::Position RazorPanel::position() const
{
    Q_D(const RazorPanel);
    return d->position();
}


/************************************************

 ************************************************/
void RazorPanel::x11EventFilter(XEvent* event)
{
    Q_D(RazorPanel);
    foreach (RazorPanelPlugin* plugin, d->mPlugins)
    {
        plugin->x11EventFilter(event);
    }
}


/************************************************

 ************************************************/
bool RazorPanel::event(QEvent* e)
{
    Q_D(RazorPanel);
    switch (e->type())
    {
        case QEvent::ContextMenu:
            d->contextMenuEvent(static_cast<QContextMenuEvent*>(e));
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
void RazorPanelPrivate::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu* menu = popupMenu(0);

    //menu->addSeparator();
    //QAction* a = menu->addAction(XdgIcon::fromTheme("application-exit"), "Exit");
    //connect(a, SIGNAL(triggered()), qApp, SLOT(quit()));

    menu->exec(event->globalPos());
    delete menu;
}


/************************************************

 ************************************************/
QMenu* RazorPanel::popupMenu(QWidget *parent) const
{
    Q_D(const RazorPanel);
    return d->popupMenu(parent);
}


/************************************************

 ************************************************/
QMenu* RazorPanelPrivate::popupMenu(QWidget *parent) const
{
    QMenu* menu = new QMenu(tr("Panel"), parent);
    menu->setIcon(XdgIcon::fromTheme("configure-toolbars"));
    QAction* a;

#ifdef DEBUG
    Q_Q(const RazorPanel);
    menu->addAction("Exit", q, SLOT(close()));
#endif

    // Plugins menu .............................
    QIcon plugIco = XdgIcon::fromTheme("preferences-plugin");
    QMenu* pluginsMenu = menu->addMenu(plugIco, tr("Plugins"));

    a = pluginsMenu->addAction(tr("Add plugins ..."));
    connect(a, SIGNAL(triggered()), this, SLOT(showAddPluginDialog()));
    pluginsMenu->addSeparator();


    foreach (RazorPanelPlugin* plugin, mPlugins)
    {
        QMenu* m = pluginsMenu->addMenu(plugin->windowTitle());

        a = new PluginAction(plugin, XdgIcon::fromTheme("transform-move"), tr("Move plugin"), m);
        connect(a, SIGNAL(triggered()), this, SLOT(onMovePlugin()));
        m->addAction(a);

        if (plugin->flags().testFlag(RazorPanelPlugin::HaveConfigDialog))
        {
            a = new PluginAction(plugin, tr("Configure plugin"), m);
            connect(a, SIGNAL(triggered()), plugin, SLOT(showConfigureDialog()));
            m->addAction(a);
        }

        m->addSeparator();

        a = new PluginAction(plugin, tr("Delete plugin"), m);
        connect(a, SIGNAL(triggered()), this, SLOT(onRemovePlugin()));
        m->addAction(a);
    }


    // Create Panel menu ********************************************
    QMenu* positionMenu = menu->addMenu(tr("Show this panel at"));
    QActionGroup* posGroup = new QActionGroup(menu);

    QDesktopWidget* dw = QApplication::desktop();
    for (int i=0; i<dw->screenCount(); ++i)
    {
        if (canPlacedOn(i, RazorPanel::PositionTop))
        {
            a = new PositionAction(i,  RazorPanel::PositionTop, posGroup);
            a->setChecked(mPosition == RazorPanel::PositionTop && mScreenNum == i);
            connect(a, SIGNAL(triggered()), this, SLOT(switchPosition()));
            positionMenu->addAction(a);
        }

        if (canPlacedOn(i, RazorPanel::PositionBottom))
        {
            a = new PositionAction(i, RazorPanel::PositionBottom, posGroup);
            a->setChecked(mPosition == RazorPanel::PositionBottom && mScreenNum == i);
            connect(a, SIGNAL(triggered()), this, SLOT(switchPosition()));
            positionMenu->addAction(a);
        }

        if (canPlacedOn(i, RazorPanel::PositionLeft))
        {
            a = new PositionAction(i, RazorPanel::PositionLeft, posGroup);
            a->setChecked(mPosition == RazorPanel::PositionLeft && mScreenNum == i);
            connect(a, SIGNAL(triggered()), this, SLOT(switchPosition()));
            positionMenu->addAction(a);
        }


        if (canPlacedOn(i, RazorPanel::PositionRight))
        {
            a = new PositionAction(i, RazorPanel::PositionRight, posGroup);
            a->setChecked(mPosition == RazorPanel::PositionRight && mScreenNum == i);
            connect(a, SIGNAL(triggered()), this, SLOT(switchPosition()));
            positionMenu->addAction(a);
        }

        positionMenu->addSeparator();
    }

    menu->addSeparator();
    a = menu->addAction(tr("Configure panel"));
    connect(a, SIGNAL(triggered()), this, SLOT(showConfigPanelDialog()));
    menu->addAction(a);

    return menu;
}

/************************************************

 ************************************************/
void RazorPanelPrivate::onMovePlugin()
{
    PluginAction* a = qobject_cast<PluginAction*>(sender());
    if (!a)
        return;

    CursorAnimation *cursorAnimation = new CursorAnimation(a->plugin());
    connect(cursorAnimation, SIGNAL(finished()), SLOT(startMoveWidget()));
    cursorAnimation->setEasingCurve(QEasingCurve::InOutQuad);
    cursorAnimation->setDuration(150);
    cursorAnimation->setStartValue(QCursor::pos());
    cursorAnimation->setEndValue(a->plugin()->mapToGlobal(a->plugin()->rect().center()));
    cursorAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void RazorPanelPrivate::startMoveWidget()
{
    CursorAnimation* a = qobject_cast<CursorAnimation*>(sender());
    if (!a)
        return;
    mLayout->startMoveWidget(qobject_cast<QWidget *>(a->parent()));
}

/************************************************

 ************************************************/
void RazorPanelPrivate::onRemovePlugin()
{
    PluginAction* a = qobject_cast<PluginAction*>(sender());
    if (!a)
        return;

    mSettings->remove(a->plugin()->configId());
    mPlugins.removeAll(a->plugin());
    delete a->plugin();
    saveSettings();
}


/************************************************

 ************************************************/
void RazorPanelPrivate::switchPosition()
{
    PositionAction* a = qobject_cast<PositionAction*>(sender());
    if (!a)
        return;

    Q_Q(RazorPanel);
    mPosition = a->position();
    mScreenNum = a->displayNum();
    realign();
    emit q->layoutDirectionChanged(q->isHorizontal() ? QBoxLayout::LeftToRight : QBoxLayout::TopToBottom);
    emit q->positionChanged();
    saveSettings();
}


/************************************************

 ************************************************/
CursorAnimation::CursorAnimation(QObject *parent):
    QVariantAnimation(parent)
{
}


/************************************************

 ************************************************/
void CursorAnimation::updateCurrentValue(const QVariant &value)
{
    QCursor::setPos(value.toPoint());
}


