/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.ru>
 *   Maciej Płaza <plaza.maciej@gmail.com>
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


#ifndef RAZORTASKBAR_H
#define RAZORTASKBAR_H

#include "../panel/razorpanelplugin.h"
#include "razortaskbarconfiguration.h"
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
    virtual RazorPanelPlugin::Flags flags() const { return HaveConfigDialog ; }

public slots:
    void activeWindowChanged();

private slots:
    void readSettings();
    void writeSettings();

protected slots:
    virtual void settigsChanged();
    virtual void showConfigureDialog();

private:
    void refreshTaskList();
    void refreshButtonVisibility();
    QHash<Window, RazorTaskButton*> mButtonsHash;
    QBoxLayout*  mLayout;
    RazorTaskButton* buttonByWindow(Window window) const;
    Window mRootWindow;
    Qt::ToolButtonStyle mButtonStyle;
    int buttonMaxWidth;
    void setButtonStyle(Qt::ToolButtonStyle buttonStyle);
    void setButtonMaxWidth(int maxWidth);
    bool mShowOnlyCurrentDesktopTasks;

    void handlePropertyNotify(XPropertyEvent* event);
};

EXPORT_RAZOR_PANEL_PLUGIN_H
#endif // RAZORTASKBAR_H
