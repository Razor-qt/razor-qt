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

#include "razorpanel.h"
#include "razorpanelplugin.h"
#include "razorpluginmanager.h"
#include "razorpanelapplication.h"

#include <razorqt/readsettings.h>


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

#include <razorqt/xdgicon.h>
#include <razorqt/xfitman.h>


#define CFG_FILE            "panel"

#define CFG_KEY_DESKTOPNUM "desktop"
#define CFG_KEY_POSITION   "position"
#define CFG_KEY_PLUGINS    "plugins"
#define CFG_KEY_STATE      "state"


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
RazorPanel::RazorPanel(QWidget *parent) :
  QMainWindow(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)

{
    setAttribute(Qt::WA_X11NetWmWindowTypeDock);
    setAttribute(Qt::WA_AlwaysShowToolTips);

    setObjectName("RazorBar");
    mPluginManager = new RazorPluginManager();

    connect(qApp, SIGNAL(x11PropertyNotify(XEvent*)), this, SIGNAL(x11PropertyNotify(XEvent*)));

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

    QString locale = QLocale::system().name();
    // Read panels & load plugins
    int cnt = settings->allKeys().count();
    for (int i=1; i<cnt; ++i)
    {
        QString pluginName = settings->value(QString("plugins/%1/name").arg(i)).toString();
        QString configId = settings->value(QString("plugins/%1/config").arg(i)).toString();

        if (pluginName.isEmpty() || configId.isEmpty())
            continue;

        QTranslator* translator = new QTranslator(this);
        translator->load(QString("%1/plugin-%2_%3.qm").arg(TRANSLATIONS_DIR, pluginName, locale));
        qApp->installTranslator(translator);
        
        
        QString soPath = QString("%1/librazorpanel_%2").arg(PLUGIN_DIR, pluginName + '2');
        RazorPanelPlugin* plugin = mPluginManager->loadPlugin(soPath, configId, this);

        if (plugin)
        {
            addToolBar(plugin);
        }
    }
            
    if (settings->contains(CFG_KEY_STATE))
        restoreState(settings->value(CFG_KEY_STATE).toByteArray());
    else
        restoreState(defaultState());

    delete panelRS;

    setTheme(mTheme);
}


/************************************************

 ************************************************/
RazorPanel::~RazorPanel()
{
    ReadSettings* panelRS = new ReadSettings("panel");
    QSettings* settings = panelRS->settings();

    settings->beginGroup(mConfigId);
    settings->setValue(CFG_KEY_DESKTOPNUM, mDesktopNum);
    settings->setValue(CFG_KEY_POSITION, positionToStr(mPosition));
    settings->setValue(CFG_KEY_STATE, saveState());

    delete panelRS;

    delete mPluginManager;
}



/************************************************

 ************************************************/
void RazorPanel::show()
{
    QMainWindow::show();
    realign();
    xfitMan().moveWindowToDesktop(this->effectiveWinId(), -1);
}


/************************************************

 ************************************************/
void debugState(const QByteArray& state, const QString logFile, bool append=true)
{
    QFile f(logFile);
    if (append)
        f.open(QFile::Append);
    else
        f.open(QFile::WriteOnly);

    f.write(state);
    f.close();
}


/************************************************

 ************************************************/
QByteArray RazorPanel::defaultState()
{
    QByteArray result = saveState();

    RazorPanelPlugin* plugin = 0;
    RazorPanelPluginIterator i(*mPluginManager);
    i.toBack();

    // Skip all right-aligned panels.
    while (i.hasPrevious())
    {
        plugin = i.previous();
        if (plugin->preferredAlignment() != RazorPanelPlugin::AlignRight)
            break;
    }

    // If exists left-aligned panel, we increase its width.
    if (plugin)
    {
        // Construct UTF-16BE string
        QByteArray baName;
        QDataStream ds(&baName, QIODevice::ReadWrite);
        ds << QString(plugin->objectName());

        int n = result.lastIndexOf(baName);
        if (n > -1)
        {
            n += baName.length(); // UTF-16BE:  Name
            n +=1;                // uchar:     1st bit: 1 if shown
                                  //            2nd bit: 1 if orientation is vert.
            n +=4;                // int:       Item.pos
            result[n]   = '\x00'; // int:       Item.Size
            result[n+1] = '\xFF';
            result[n+2] = '\xFF';
            result[n+3] = '\xFF';
        }

    }

    return result;
}


/************************************************

 ************************************************/
void RazorPanel::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu menu(tr("Panel"));
    QMenu* m;
    QAction* a;

    m = menu.addMenu(XdgIcon::fromTheme("preferences-plugin", 32), tr("Plugins"));

    for (int i=0; i<mPluginManager->count(); ++i)
    {
        const RazorPanelPlugin* plugin = mPluginManager->at(i);
        QMenu* plugMenu = m->addMenu(plugin->windowTitle());
        plugMenu->setIcon(plugin->windowIcon());

        if (plugin->isMovable())
            a = plugMenu->addAction(XdgIcon::fromTheme("document-encrypt", 32), tr("Lock"));
        else
            a = plugMenu->addAction(XdgIcon::fromTheme("document-decrypt", 32), tr("Unlock"));

        a->setData(i);
        connect(a, SIGNAL(triggered()), this, SLOT(lockPlugin()));

        if (plugin->isVisible())
        {
            a = plugMenu->addAction(XdgIcon::fromTheme("layer-visible-off", 32), tr("Hide"));
            connect(a, SIGNAL(triggered()), plugin, SLOT(hide()));
            a->setData(i);
        }
        else
        {
            a = plugMenu->addAction(XdgIcon::fromTheme("layer-visible-on", 32), tr("Show"));
            connect(a, SIGNAL(triggered()), plugin, SLOT(show()));
            a->setData(i);
        }
    }


    // Create Panel menu ********************************************
    m = menu.addMenu(XdgIcon::fromTheme("configure-toolbars", 32), tr("Show this panel at"));
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

    menu.addSeparator();
    a = menu.addAction(XdgIcon::fromTheme("application-exit", 32), "Exit");
    connect(a, SIGNAL(triggered()), this, SLOT(close()));

    menu.exec(event->globalPos());

}


/************************************************

 ************************************************/
void RazorPanel::lockPlugin()
{
    QAction* a = qobject_cast<QAction*>(sender());
    if (!a)
        return;

    bool ok;
    int n = a->data().toInt(&ok);
    if (ok && n>-1 && n<mPluginManager->count())
    {
        RazorPanelPlugin* plugin = mPluginManager->value(n);
        plugin->setMovable(!plugin->isMovable());
    }
}


/************************************************

 ************************************************/
void RazorPanel::switchPosition()
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
RazorPanel::Position RazorPanel::strToPosition(const QString& str, Position defaultValue) const
{
    if (str.toUpper() == "TOP")    return PositionTop;
    if (str.toUpper() == "LEFT")   return PositionLeft;
    if (str.toUpper() == "RIGHT")  return PositionRight;
    if (str.toUpper() == "BOTTOM") return PositionBottom;
    return defaultValue;
}


/************************************************

 ************************************************/
QString RazorPanel::positionToStr(Position position) const
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
void RazorPanel::setTheme(const QString& themeName)
{
    mTheme = themeName;
    ReadTheme* readTheme = new ReadTheme(themeName);

    qApp->setStyleSheet(readTheme->qss());

    delete readTheme;

    realign();
}


/************************************************

 ************************************************/
void RazorPanel::realign()
{
    /*
    qDebug() << "Realign: DesktopNum" << mDesktopNum;
    qDebug() << "Realign: Position  " << positionToStr(mPosition);
    qDebug() << "Realign: Theme     " << mTheme;
    qDebug() << "Realign: SizeHint  " << sizeHint();
    qDebug() << "Realign: Screen    " << QApplication::desktop()->screenGeometry(mDesktopNum);
    */

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

    setGeometry(rect);


    XfitMan xf = xfitMan();
    //reserve our space on the screen
    Window wid = this->effectiveWinId();

    switch (mPosition)
    {
        case PositionTop:
            xf.setStrut(wid, 0, 0, height(), 0,
               /* Left   */   0, 0,
               /* Right  */   0, 0,
               /* Top    */   rect.left(), rect.right(),
               /* Bottom */   0, 0
                         );
        break;

        case PositionBottom:
            xf.setStrut(wid, 0, 0, 0, height(),
               /* Left   */   0, 0,
               /* Right  */   0, 0,
               /* Top    */   0, 0,
               /* Bottom */   rect.left(), rect.right()
                         );
            break;

        case PositionLeft:
            xf.setStrut(wid, width(), 0, 0, 0,
               /* Left   */   rect.top(), rect.bottom(),
               /* Right  */   0, 0,
               /* Top    */   0, 0,
               /* Bottom */   0, 0
                         );

            break;

        case PositionRight:
            xf.setStrut(wid, 0, width(), 0, 0,
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
bool RazorPanel::canPlacedOn(int displayNum, RazorPanel::Position position) const
{
    QDesktopWidget* dw = QApplication::desktop();

    switch (position)
    {
        case RazorPanel::PositionTop:
            for (int i=0; i < dw->screenCount(); ++i)
                if (dw->screenGeometry(i).bottom() < dw->screenGeometry(displayNum).top())
                    return false;
                return true;

        case RazorPanel::PositionBottom:
            for (int i=0; i < dw->screenCount(); ++i)
                if (dw->screenGeometry(i).top() > dw->screenGeometry(displayNum).bottom())
                    return false;
                return true;

        case RazorPanel::PositionLeft:
//            for (int i=0; i < dw->screenCount(); ++i)
//                if (dw->screenGeometry(i).right() < dw->screenGeometry(displayNum).left())
//                    return false;
//            return true;
            return false;

        case RazorPanel::PositionRight:
//            for (int i=0; i < dw->screenCount(); ++i)
//                if (dw->screenGeometry(i).left() > dw->screenGeometry(displayNum).right())
//                    return false;
//            return true;
            return false;
    }

}
