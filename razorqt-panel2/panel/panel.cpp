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

#include <QtGui/QAction>
#include <QtGui/QActionGroup>


#define CFG_FILE            "panel"

#define CFG_KEY_DESKTOPNUM "desktop"
#define CFG_KEY_POSITION   "position"
#define CFG_KEY_PLUGINS    "plugins"
#define CFG_KEY_STATE      "state"

/************************************************

 ************************************************/
PositionAction::PositionAction(int displayNum, Panel::Position position, QActionGroup *parent):
    QAction(parent)
{
    if (QApplication::desktop()->screenCount() == 1)
    {
        switch (position)
        {
            case Panel::PositionTop:    setText(tr("Top of desktop"));      break;
            case Panel::PositionBottom: setText(tr("Bottom of desktop"));   break;
            case Panel::PositionLeft:   setText(tr("Left of desktop"));     break;
            case Panel::PositionRight:  setText(tr("Right of desktop"));    break;
        }
    }
    else
    {
        switch (position)
        {
        case Panel::PositionTop:    setText(tr("Top of desktop %1").arg(displayNum +1));    break;
        case Panel::PositionBottom: setText(tr("Bottom of desktop %1").arg(displayNum +1)); break;
        case Panel::PositionLeft:   setText(tr("Left of desktop %1").arg(displayNum +1));   break;
        case Panel::PositionRight:  setText(tr("Right of desktop %1").arg(displayNum +1));  break;
        }
    }


    mPosition = position;
    mDisplayNum = displayNum;
    setCheckable(true);
    parent->addAction(this);
}



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
    mConfigId = "default";
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

    restoreState(settings->value(CFG_KEY_STATE).toByteArray());

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
    settings->setValue(CFG_KEY_STATE, saveState());

    delete panelRS;

    delete mPluginManager;
    delete mXfitMan;
}


/************************************************

 ************************************************/
void Panel::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu menu(tr("Panel"));

    QMenu* m;
    QAction* a;

    m = menu.addMenu(tr("Plugins"));

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


    // Create Panel menu ********************************************
    m = menu.addMenu(tr("Show this panel at"));
    QActionGroup posGroup(&menu);

    QDesktopWidget* dw = QApplication::desktop();
    for (int i=0; i<dw->screenCount(); ++i)
    {
        if (canPlacedOn(i, PositionTop))
        {
            a = new PositionAction(i,  PositionTop, &posGroup);
            a->setChecked(mPosition == PositionTop && mDesktopNum == i);
            connect(a, SIGNAL(triggered()), this, SLOT(switchPosition()));
            m->addAction(a);
        }

        if (canPlacedOn(i, PositionBottom))
        {
            a = new PositionAction(i, PositionBottom, &posGroup);
            a->setChecked(mPosition == PositionBottom && mDesktopNum == i);
            connect(a, SIGNAL(triggered()), this, SLOT(switchPosition()));
            m->addAction(a);
        }

        if (canPlacedOn(i, PositionLeft))
        {
            a = new PositionAction(i, PositionLeft, &posGroup);
            a->setChecked(mPosition == PositionLeft && mDesktopNum == i);
            connect(a, SIGNAL(triggered()), this, SLOT(switchPosition()));
            m->addAction(a);
        }


        if (canPlacedOn(i, PositionRight))
        {
            a = new PositionAction(i, PositionRight, &posGroup);
            a->setChecked(mPosition == PositionRight && mDesktopNum == i);
            connect(a, SIGNAL(triggered()), this, SLOT(switchPosition()));
            m->addAction(a);
        }

        m->addSeparator();
    }
    // End of create Panel menu *************************************

    a = menu.addAction("Exit");
    connect(a, SIGNAL(triggered()), this, SLOT(close()));

    menu.exec(mapToGlobal(event->pos()));

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
    PositionAction* a = qobject_cast<PositionAction*>(sender());
    if (!a)
        return;

    mPosition = a->position();
    mDesktopNum = a->displayNum();
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
    qDebug() << "Realign: SizeHint  " << sizeHint();
    qDebug() << "Realign: Screen    " << QApplication::desktop()->screenGeometry(mDesktopNum);

    QRect screen = QApplication::desktop()->screenGeometry(mDesktopNum);
    QRect rect = screen;
    switch (mPosition)
    {
        case PositionTop:
            rect.setHeight(0);
            break;

        case PositionBottom:
            rect.setHeight(0);
            rect.moveTop(screen.bottom() - sizeHint().height());
            break;

        case PositionLeft:
            rect.setWidth(0);
            break;

        case PositionRight:
            rect.setWidth(0);
            rect.moveLeft(screen.right() - sizeHint().width());
            break;
    }

    qDebug() << "Realign: New Pos   " << rect;
    setGeometry(rect);
    qDebug() << "Realign: Size      " << size();

/*
    switch (mPosition)
    {
        case PositionTop:
            rect.setHeight(0);
            break;

        case PositionBottom:
            rect.setHeight(0);
            rect.moveTop(screen.bottom() - sizeHint().height());
            break;

        case PositionLeft:
            rect.setWidth(0);
            break;

        case PositionRight:
            rect.setWidth(0);
            rect.moveLeft(screen.right() - size().width());
            break;
    }
    setGeometry(rect);
*/


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


/************************************************

 ************************************************/
bool Panel::canPlacedOn(int displayNum, Panel::Position position) const
{
    QDesktopWidget* dw = QApplication::desktop();

    switch (position)
    {
        case Panel::PositionTop:
            for (int i=0; i < dw->screenCount(); ++i)
                if (dw->screenGeometry(i).bottom() < dw->screenGeometry(displayNum).top())
                    return false;
                return true;

        case Panel::PositionBottom:
            for (int i=0; i < dw->screenCount(); ++i)
                if (dw->screenGeometry(i).top() > dw->screenGeometry(displayNum).bottom())
                    return false;
                return true;

        case Panel::PositionLeft:
//            for (int i=0; i < dw->screenCount(); ++i)
//                if (dw->screenGeometry(i).right() < dw->screenGeometry(displayNum).left())
//                    return false;
//            return true;
            return false;

        case Panel::PositionRight:
//            for (int i=0; i < dw->screenCount(); ++i)
//                if (dw->screenGeometry(i).left() > dw->screenGeometry(displayNum).right())
//                    return false;
//            return true;
            return false;
    }

}
