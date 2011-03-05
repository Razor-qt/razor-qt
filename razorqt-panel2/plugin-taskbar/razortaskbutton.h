/********************************************************************
  Copyright: 2011 Alexander Sokoloff <sokoloff.a@gmail.ru>

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

#ifndef RAZORTASKBUTTON_H
#define RAZORTASKBUTTON_H

#include <QtGui/QButtonGroup>
#include <QtGui/QToolButton>
#include <QtCore/QHash>

//namespace X11
//{
//    extern "C"
//    {
        #include <X11/X.h>
//        #include <X11/Xlib.h>
//    }
//};


class RazorTaskButton : public QToolButton
{
    Q_OBJECT
public:
    explicit RazorTaskButton(const Window window, QWidget *parent = 0);
    virtual ~RazorTaskButton();

    bool isAppHidden() const;
    bool isApplicationActive() const;
    Window windowId() const { return mWindow; }

public slots:
    void raiseApplication();
    void minimizeApplication();
    void closeApplication();
    //void moveApplicationToDesktop();

protected:
    void nextCheckState();
    void contextMenuEvent( QContextMenuEvent* event);

private:
    Window mWindow;
    static QButtonGroup mButtonsGroup;

private slots:
    void btnClicked(bool checked);
  //  void handlePropertyNotify(XPropertyEvent* event);
};

typedef QHash<Window,RazorTaskButton*> RazorTaskButtonHash;

#endif // RAZORTASKBUTTON_H
