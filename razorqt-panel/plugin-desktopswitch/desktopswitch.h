/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
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


#ifndef DESKTOPSWITCH_H
#define DESKTOPSWITCH_H

#include "../panel/irazorpanelplugin.h"
#include <QFrame>

class QSignalMapper;
class QButtonGroup;
class RazorGridLayout;

class DesktopSwitchWidget: public QFrame
{
    Q_OBJECT
public:
    DesktopSwitchWidget();

protected:
    void wheelEvent(QWheelEvent* e);
};

/**
 * @brief Desktop switcher. A very simple one...
 */
class DesktopSwitch : public QObject, public IRazorPanelPlugin
{
    Q_OBJECT
public:
    DesktopSwitch(const IRazorPanelPluginStartupInfo &startupInfo);
    ~DesktopSwitch();

    QString themeId() const { return "DesktopSwitch"; }
    QWidget *widget() { return &mWidget; }
    bool isSeparate() const { return true; }
    virtual void x11EventFilter(XEvent* event);
    void realign();

private:
    QButtonGroup * m_buttons;
    QSignalMapper* m_pSignalMapper;
    int m_desktopCount;
    QStringList m_desktopNames;
    DesktopSwitchWidget mWidget;
    RazorGridLayout *mLayout;

    void setup();

private slots:
    void setDesktop(int desktop);

protected slots:

};


class DesktopSwitchPluginLibrary: public QObject, public IRazorPanelPluginLibrary
{
    Q_OBJECT
    Q_INTERFACES(IRazorPanelPluginLibrary)
public:
    IRazorPanelPlugin *instance(const IRazorPanelPluginStartupInfo &startupInfo) { return new DesktopSwitch(startupInfo);}
};

#endif
