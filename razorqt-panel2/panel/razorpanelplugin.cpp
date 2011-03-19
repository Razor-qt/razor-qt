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
//#include <QStyleOptionToolBar>

#include "razorpanelplugin.h"
#include "razorpanellayout.h"

//#include <QToolButton>
#include <QApplication>
#include <QDebug>
#include <QtCore/QEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QMenu>
#include <razorqt/xdgicon.h>

/************************************************

 ************************************************/
RazorPanelPlugin::RazorPanelPlugin(RazorPanel* panel, const QString& configId, QWidget *parent) :
    QFrame(parent),
    mPanel(panel),
    mConfigId(configId)
{
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    mLayout = new QBoxLayout(QBoxLayout::LeftToRight, this);
    mLayout->setSpacing(0);
    mLayout->setMargin(0);
    mLayout->setContentsMargins(0, 0, 0, 0);
}


/************************************************

 ************************************************/
RazorPanelPlugin::~RazorPanelPlugin()
{

}


/************************************************

 ************************************************/
void RazorPanelPlugin::addWidget(QWidget* widget)
{
    mLayout->addWidget(widget);
}


/************************************************

 ************************************************/
void RazorPanelPlugin::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu* menu = popupMenu(0);
    menu->exec(event->globalPos());
    delete menu;
}


/************************************************

 ************************************************/
QMenu* RazorPanelPlugin::popupMenu(QWidget *parent)
{
    QMenu* menu = new QMenu(parent);
    QAction* a;

    a = menu->addAction(XdgIcon::fromTheme("transform-move", 32), tr("Move plugin"));
    connect(a, SIGNAL(triggered()), this, SLOT(startMove()));

    menu->addSeparator();
    menu->addMenu(panel()->popupMenu(menu));

    menu->addSeparator();
    a = menu->addAction(XdgIcon::fromTheme("application-exit", 32), "Exit");
    connect(a, SIGNAL(triggered()), qApp, SLOT(quit()));

    return menu;
}


/************************************************

 ************************************************/
void RazorPanelPlugin::startMove()
{
    RazorPanelLayout* layout = qobject_cast<RazorPanelLayout*>(panel()->layout());
    if (!layout)
        return;

    qDebug() << "START";
}


/************************************************

 ************************************************/


/************************************************
  Workaround about QTBUG-597
  http://bugreports.qt.nokia.com/browse/QTBUG-597
 ************************************************/
//bool RazorPanelPlugin::eventFilter(QObject* watched, QEvent* event)
//{

//    if (watched == mExtensionButton)
//    {
//        switch (event->type())
//        {
//            case QEvent::MouseButtonPress:
//            case QEvent::MouseButtonDblClick:
//                return true;

//            case QEvent::MouseButtonRelease:
//                showExtensionMenu((QMouseEvent*) event);
//                return true;

//            default:
//                break;

//        }
//    }
//    return QToolBar::eventFilter(watched, event);
//}


/************************************************
  Workaround about QTBUG-597
  http://bugreports.qt.nokia.com/browse/QTBUG-597
 ************************************************/
//void RazorPanelPlugin::showExtensionMenu(QMouseEvent* event)
//{
//    QMenu menu;
//    QPoint extBtnPos = this->mExtensionButton->pos();

//    QList<QAction*> acts = actions();

//    switch (this->orientation())
//    {
//        case Qt::Horizontal:
//            foreach(QAction* act, acts)
//            {
//                if (actionGeometry(act).right() > extBtnPos.x())
//                    menu.addAction(act);
//            }
//            break;

//        case Qt::Vertical:
//            foreach(QAction* act, acts)
//            {
//                if (actionGeometry(act).bottom() > extBtnPos.y())
//                    menu.addAction(act);
//            }
//            break;
//    }

//    menu.exec(event->globalPos());
//}
