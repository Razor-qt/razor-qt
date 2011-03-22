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

#include <QStyleOptionToolBar>
#include <QPainter>
#include <QToolTip>
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
    mConfigId(configId),
    mMovable(false)
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
void RazorPanelPlugin::initStyleOption(QStyleOptionToolBar *option) const
{
    option->initFrom(this);
    option->state |= QStyle::State_Horizontal;
    if (mMovable)
        option->features = QStyleOptionToolBar::Movable;
}


/************************************************

 ************************************************/
QRect RazorPanelPlugin::handleRect()
{
    QStyleOptionToolBar opt;
    initStyleOption(&opt);

    return style()->subElementRect(QStyle::SE_ToolBarHandle, &opt, this);
}


/************************************************

 ************************************************/
void RazorPanelPlugin::setMovable(bool movable)
{
    if (mMovable == movable)
        return;

    mMovable = movable;

    QMargins m = contentsMargins();

    if (movable)
        m.setLeft(m.left() + handleRect().width());
    else
        m.setLeft(m.left() - handleRect().width());

    setContentsMargins(m);


    // Update stylesheet ............
    style()->unpolish(this);
    style()->polish(this);
}


/************************************************

 ************************************************/
bool RazorPanelPlugin::isMovable() const
{
    return mMovable;
}


/************************************************

 ************************************************/
void RazorPanelPlugin::toggleMovable()
{
    setMovable(!isMovable());
}

///************************************************

// ************************************************/
//void RazorPanelPlugin::mousePressEvent(QMouseEvent* event)
//{
//     static_cast<RazorPanelLayout*>(panel()->layout())->startMoveWidget(this);
//}

////    if (handleRect().contains(event->pos()))
////    {
////        panel()->layout()->startMoveWidget(this);
////        event->accept();
////    }
////}


/************************************************

 ************************************************/
//void RazorPanelPlugin::mouseReleaseEvent(QMouseEvent* event)
//{
//        RazorPanelLayout* l = qobject_cast<RazorPanelLayout*>(panel()->layout());
//        if (!l)
//            return;
//        qDebug() << "STOP";
//        event->accept(); // do not propagate
////        mOffset = QPoint();
////        int myN = l->indexOf(this);
////        l->moveItem(myN, 2);
//        panel()->layout()->setEnabled(true);
//        panel()->layout()->invalidate();


//   static_cast<RazorPanelLayout*>(panel()->layout())->stopMoveWidget();
//}


/************************************************

 ************************************************/
void RazorPanelPlugin::startMove()
{
    static_cast<RazorPanelLayout*>(panel()->layout())->startMoveWidget(this);
}

//    return;
//    RazorPanelLayout* layout = qobject_cast<RazorPanelLayout*>(panel()->layout());
//    if (!layout)
//        return;
////this->setToolTip("Move me");
////this->toolTip()
//    QMargins m = contentsMargins();
//    m.setLeft(m.left() + 10);
//    setContentsMargins(m);

//    QToolTip::showText(mapToGlobal(QPoint(this->pos())), "Move me");
//    layout->startMoveWidget(this);
//panel()->layout()->setEnabled(false);
//move(0, 10);
//setParent(0);
//show();
//}




//void RazorPanelPlugin::initStyleOption(QStyleOptionToolBar *option) const
//{
//    //Q_D(const QToolBar);

//    if (!option)
//        return;

//    option->initFrom(this);
//    //if (orientation() == Qt::Horizontal)
//    //    option->state |= QStyle::State_Horizontal;
//    option->lineWidth = style()->pixelMetric(QStyle::PM_ToolBarFrameWidth, 0, this);
//    option->features = d->layout->movable()
//                        ? QStyleOptionToolBar::Movable
//                        : QStyleOptionToolBar::None;
//    // if the tool bar is not in a QMainWindow, this will make the painting right
//    option->toolBarArea = Qt::NoToolBarArea;

//    // Add more styleoptions if the toolbar has been added to a mainwindow.
//    QMainWindow *mainWindow = qobject_cast<QMainWindow *>(parentWidget());

//    if (!mainWindow)
//        return;

//    QMainWindowLayout *layout = qobject_cast<QMainWindowLayout *>(mainWindow->layout());
//    Q_ASSERT_X(layout != 0, "QToolBar::initStyleOption()",
//               "QMainWindow->layout() != QMainWindowLayout");

//    layout->getStyleOptionInfo(option, const_cast<QToolBar *>(this));
//}

/************************************************

 ************************************************/
void RazorPanelPlugin::paintEvent(QPaintEvent* event)
{
    if (mMovable)
    {
        QPainter p(this);
        QStyle *style = this->style();
        QStyleOptionToolBar opt;
        initStyleOption(&opt);

        opt.rect = style->subElementRect(QStyle::SE_ToolBarHandle, &opt, this);
        if (opt.rect.isValid())
            style->drawPrimitive(QStyle::PE_IndicatorToolBarHandle, &opt, &p, this);
    }
}

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
