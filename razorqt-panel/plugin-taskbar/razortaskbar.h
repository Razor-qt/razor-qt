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

#ifndef RAZORTASKBAR_H
#define RAZORTASKBAR_H

#include "../panel/razorpanelplugin.h"
#include <QtCore/QObject>
#include <QtCore/QHash>
#include <X11/Xlib.h>

class RazorTaskButton;


class RazorTaskBar : public RazorPanelPlugin
{
    Q_OBJECT
public:
    explicit RazorTaskBar(const RazorPanelPluginStartInfo* startInfo, QWidget* parent = 0);
    virtual ~RazorTaskBar();

    virtual void x11EventFilter(XEvent* event);

public slots:
    void activeWindowChanged();


private:
    void refreshTaskList();
    QHash<Window, RazorTaskButton*> mButtonsHash;
    RazorTaskButton* buttonByWindow(Window window) const;
    Window mRootWindow;


    void handlePropertyNotify(XPropertyEvent* event);
};

EXPORT_RAZOR_PANEL_PLUGIN_H
#endif // RAZORTASKBAR_H
