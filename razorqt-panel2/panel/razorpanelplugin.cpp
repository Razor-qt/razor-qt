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
#include <QStyleOptionToolBar>

#include "razorpanelplugin.h"
#include <QToolButton>
#include <QDebug>
#include <QtCore/QEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QMenu>

/************************************************

 ************************************************/
RazorPanelPlugin::RazorPanelPlugin(RazorPanel* panel, const QString& configId, QWidget *parent) :
    QToolBar(parent)
{
    mPanel = panel;
    mConfigId = configId;
    setFloatable(false);


    mExtensionButton = findChild<QToolButton*>(QLatin1String("qt_toolbar_ext_button"));
    if (mExtensionButton)
        mExtensionButton->installEventFilter(this);

}


/************************************************

 ************************************************/
RazorPanelPlugin::~RazorPanelPlugin()
{

}


/************************************************
  Workaround about QTBUG-597
  http://bugreports.qt.nokia.com/browse/QTBUG-597
 ************************************************/
bool RazorPanelPlugin::eventFilter(QObject* watched, QEvent* event)
{

    if (watched == mExtensionButton)
    {
        switch (event->type())
        {
            case QEvent::MouseButtonPress:
            case QEvent::MouseButtonDblClick:
                return true;

            case QEvent::MouseButtonRelease:
                showExtensionMenu((QMouseEvent*) event);
                return true;

            default:
                break;

        }
    }
    return QToolBar::eventFilter(watched, event);
}


/************************************************
  Workaround about QTBUG-597
  http://bugreports.qt.nokia.com/browse/QTBUG-597
 ************************************************/
void RazorPanelPlugin::showExtensionMenu(QMouseEvent* event)
{
    QMenu menu;
    QPoint extBtnPos = this->mExtensionButton->pos();

    QList<QAction*> acts = actions();

    switch (this->orientation())
    {
        case Qt::Horizontal:
            foreach(QAction* act, acts)
            {
                if (actionGeometry(act).right() > extBtnPos.x())
                    menu.addAction(act);
            }
            break;

        case Qt::Vertical:
            foreach(QAction* act, acts)
            {
                if (actionGeometry(act).bottom() > extBtnPos.y())
                    menu.addAction(act);
            }
            break;
    }

    menu.exec(event->globalPos());
}
