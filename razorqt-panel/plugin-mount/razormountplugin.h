/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
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


#ifndef RAZORMOUNTPLUGIN_H
#define RAZORMOUNTPLUGIN_H

#include "../panel/irazorpanelplugin.h"
#include "../panel/razorpanel.h"

class RazorMountManager;
class Popup;
class MountButton;
class DeviceAction;
class QIcon;

/*! 
\author Petr Vanek <petr@scribus.info>
*/
class RazorMountPlugin : public QObject, public IRazorPanelPlugin
{
    Q_OBJECT

public:
    RazorMountPlugin(const IRazorPanelPluginStartupInfo &startupInfo);
    ~RazorMountPlugin();

    virtual QWidget *widget();
    virtual QString themeId() const { return "RazorMount"; }
    virtual IRazorPanelPlugin::Flags flags() const { return PreferRightAlignment | HaveConfigDialog ; }
    void realign();
    QDialog *configureDialog();

    RazorMountManager *mountManager() const { return mMountManager; }
    Popup *popup() { return mPopup; }

    QIcon icon() const;

protected slots:
    virtual void settingsChanged();

private:
    MountButton *mButton;
    Popup *mPopup;
    RazorMountManager *mMountManager;
    DeviceAction *mDeviceAction;
};


class RazorMountPluginLibrary: public QObject, public IRazorPanelPluginLibrary
{
    Q_OBJECT
    Q_INTERFACES(IRazorPanelPluginLibrary)
public:
    IRazorPanelPlugin *instance(const IRazorPanelPluginStartupInfo &startupInfo)
    {
        return new RazorMountPlugin(startupInfo);
    }
};

#endif
