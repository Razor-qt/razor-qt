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

#include "panel.h"
#include "razorpanelplugin.h"
#include "razorpluginmanager.h"

#include <razorqt/readsettings.h>
#include <razorqt/xfitman.h>

#include <QDebug>
#include <QApplication>
#include <QDesktopWidget>
#include <QRect>
#include <QMenu>
#include <QContextMenuEvent>

#include <QAction>


#define CFG_FILE            "panel"

#define CFG_KEY_DESKTOPNUM "desktop"
#define CFG_KEY_POSITION   "position"
#define CFG_KEY_PLUGINS    "plugins"


/************************************************

 ************************************************/
Panel::Panel(QWidget *parent) :
  QMainWindow(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)
{
    setAttribute(Qt::WA_X11NetWmWindowTypeDock);
    setObjectName("RazorBar");
    mPluginManager = new RazorPluginManager();
    mXfitMan = new XfitMan();

    // Read command line arguments .....................
    mConfigId = "main";
    if (qApp->arguments().count() > 1)
    {
        // First argument is config file section name.
        mConfigId = qApp->arguments().at(1);
    }



    ReadSettings* razorRS = new ReadSettings("razor");
    QSettings* razorSettings = razorRS->settings();
    mTheme = razorSettings->value("theme").toString();
    delete razorRS;

    ReadSettings* panelRS = new ReadSettings(CFG_FILE);
    QSettings* settings = panelRS->settings();


    settings->beginGroup(mConfigId);
    mPosition = strToPosition(settings->value(CFG_KEY_POSITION).toString(), PositionBottom);
    mDesktopNum = settings->value(CFG_KEY_DESKTOPNUM, QApplication::desktop()->primaryScreen()).toInt();

    // Read panels & load plugins
    QStringList keys = settings->allKeys();
    foreach (QString key, keys)
    {
        if (key.startsWith(QString("%1/").arg(CFG_KEY_PLUGINS)) && key.endsWith("/name"))
        {
            QString name = settings->value(key, "").toString();
            if (name.isEmpty())
                continue;

            QString soPath = QString("%1/librazorpanel_%2.so").arg(PLUGIN_DIR, name);
            RazorPanelPlugin* plugin = mPluginManager->loadPlugin(soPath);
            if (plugin)
                addToolBar(plugin);
        }
    }


    delete panelRS;

    setTheme(mTheme);
}


/************************************************

 ************************************************/
void Panel::show()
{
    QMainWindow::show();
    realign();
    mXfitMan->moveWindowtoDesktop(this->effectiveWinId(), -1);
}


/************************************************

 ************************************************/
Panel::~Panel()
{
    ReadSettings* panelRS = new ReadSettings("panel");
    QSettings* settings = panelRS->settings();

    settings->beginGroup(mConfigId);
    settings->setValue(CFG_KEY_DESKTOPNUM, mDesktopNum);
    settings->setValue(CFG_KEY_POSITION, positionToStr(mPosition));
    delete panelRS;

    delete mPluginManager;
    delete mXfitMan;

//    settings.setValue("windowState", saveState());
}


/************************************************

 ************************************************/
void Panel::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu* menu = new QMenu(tr("Panel"));

    QMenu* m;
    QAction* a;

    m = menu->addMenu(tr("Plugins"));

    for (int i=0; i<mPluginManager->count(); ++i)
    {
        const RazorPanelPlugin* plugin = mPluginManager->at(i);
        QMenu* plugMenu = m->addMenu(plugin->windowTitle());
        m->setIcon(plugin->windowIcon());

        a = plugMenu->addAction(tr("Locked"));
        a->setData(i);
        a->setCheckable(true);
        a->setChecked(!plugin->isMovable());
        connect(a, SIGNAL(toggled(bool)), this, SLOT(lockPlugin(bool)));

        a = plugMenu->addAction(tr("Visible"));
        a->setData(i);
        a->setCheckable(true);
        a->setChecked(plugin->isVisible());
        connect(a, SIGNAL(triggered(bool)), plugin, SLOT(setVisible(bool)));
    }

    QMenu* panelMenu = menu->addMenu(tr("Panel"));

    m = panelMenu->addMenu(tr("Position"));

    a = m->addAction(tr("Top"));
    a->setCheckable(true);
    a->setData(PositionTop);
    a->setChecked(mPosition == PositionTop);
    connect(a, SIGNAL(triggered()), this, SLOT(switchPosition()));

    a = m->addAction(tr("Bottom"));
    a->setCheckable(true);
    a->setData(PositionBottom);
    a->setChecked(mPosition == PositionBottom);
    connect(a, SIGNAL(triggered()), this, SLOT(switchPosition()));

/*
    // Left & right alignment have work bagly, disabled it till.

    a = m->addAction(tr("Left"));
    a->setCheckable(true);
    a->setData(PositionLeft);
    a->setChecked(mPosition == PositionLeft);
    connect(a, SIGNAL(triggered()), this, SLOT(switchPosition()));

    a = m->addAction(tr("Right"));
    a->setCheckable(true);
    a->setData(PositionRight);
    a->setChecked(mPosition == PositionRight);
    connect(a, SIGNAL(triggered()), this, SLOT(switchPosition()));
*/

    m = panelMenu->addMenu(tr("Show on desktop"));
    for (int i=0; i< QApplication::desktop()->screenCount(); ++i)
    {
        a = m->addAction(tr("Show on desctop %1").arg(i));
        a->setCheckable(true);
        a->setChecked(i == mDesktopNum);
        a->setData(i);
        connect(a, SIGNAL(triggered()), this, SLOT(switchDesktop()));
    }


    a = menu->addAction("Exit");
    connect(a, SIGNAL(triggered()), this, SLOT(close()));

    menu->exec(mapToGlobal(event->pos()));

}


/************************************************

 ************************************************/
void Panel::lockPlugin(bool value)
{
    QAction* a = qobject_cast<QAction*>(sender());
    if (!a)
        return;

    bool ok;
    int n = a->data().toInt(&ok);
    if (ok && n>-1 && n<mPluginManager->count())
    {
        RazorPanelPlugin* plugin = mPluginManager->value(n);
        plugin->setMovable(!value);
    }
}


/************************************************

 ************************************************/
void Panel::switchPosition()
{
    QAction* a = qobject_cast<QAction*>(sender());
    if (!a)
        return;

    mPosition = (Position)a->data().toInt();
    realign();
}


/************************************************

 ************************************************/
void Panel::switchDesktop()
{
    QAction* a = qobject_cast<QAction*>(sender());
    if (!a)
        return;

    mDesktopNum = a->data().toInt();
    realign();
}


/************************************************

 ************************************************/
Panel::Position Panel::strToPosition(const QString& str, Position defaultValue) const
{
    if (str.toUpper() == "TOP")    return PositionTop;
    if (str.toUpper() == "LEFT")   return PositionLeft;
    if (str.toUpper() == "RIGHT")  return PositionRight;
    if (str.toUpper() == "BOTTOM") return PositionBottom;
    return defaultValue;
}


/************************************************

 ************************************************/
QString Panel::positionToStr(Position position) const
{
    switch (position)
    {
        case PositionTop:    return QString("Top");
        case PositionLeft:   return QString("Left");
        case PositionRight:  return QString("Right");
        case PositionBottom: return QString("Bottom");
    }
}


/************************************************

 ************************************************/
void Panel::setTheme(const QString& themeName)
{
    mTheme = themeName;
    ReadTheme* readTheme = new ReadTheme(themeName);

    qApp->setStyleSheet(readTheme->qss());

    delete readTheme;

    realign();
}


/************************************************

 ************************************************/
void Panel::setDesktopNum(int desktopNum)
{
    mDesktopNum = desktopNum;
    realign();
}


/************************************************

 ************************************************/
void Panel::setPosition(Position position)
{
    mPosition = position;
    realign();
}


/************************************************

 ************************************************/
void Panel::realign()
{
    qDebug() << "Realign: DesktopNum" << mDesktopNum;
    qDebug() << "Realign: Position  " << positionToStr(mPosition);
    qDebug() << "Realign: Theme     " << mTheme;
    qDebug() << "Realign: Height    " << height();
    qDebug() << "Realign: HeightHint" << sizeHint();


    QRect screen = QApplication::desktop()->screenGeometry(mDesktopNum);
    QRect rect = screen;
    switch (mPosition)
    {
        case PositionTop:
            rect.setHeight(0);
            break;

        case PositionBottom:
            rect.setHeight(0);
            rect.setTop(screen.height() - sizeHint().height());
            break;

        case PositionLeft:
            rect.setWidth(0);
            break;

        case PositionRight:
            rect.setWidth(0);
            rect.setRight(screen.width() - sizeHint().width());
            break;
    }

    setGeometry(rect);

    //reserve our space on the screen
    Window wid = this->effectiveWinId();

    switch (mPosition)
    {
        case PositionTop:
            mXfitMan->setStrut(wid, 0, 0, height(), 0,
                     /* Left   */   0, 0,
                     /* Right  */   0, 0,
                     /* Top    */   rect.left(), rect.right(),
                     /* Bottom */   0, 0
                               );
        break;

        case PositionBottom:
            mXfitMan->setStrut(wid, 0, 0, 0, height(),
                     /* Left   */   0, 0,
                     /* Right  */   0, 0,
                     /* Top    */   0, 0,
                     /* Bottom */   rect.left(), rect.right()
                               );
            break;

        case PositionLeft:
            mXfitMan->setStrut(wid, width(), 0, 0, 0,
                     /* Left   */   rect.top(), rect.bottom(),
                     /* Right  */   0, 0,
                     /* Top    */   0, 0,
                     /* Bottom */   0, 0
                               );

            break;

        case PositionRight:
            mXfitMan->setStrut(wid, 0, width(), 0, 0,
                     /* Left   */   0, 0,
                     /* Right  */   rect.top(), rect.bottom(),
                     /* Top    */   0, 0,
                     /* Bottom */   0, 0
                               );
            break;
    }

}

