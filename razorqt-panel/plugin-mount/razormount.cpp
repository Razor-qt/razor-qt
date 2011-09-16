/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation;  only version 2 of
 * the License is valid for this program.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * END_COMMON_COPYRIGHT_HEADER */


#include "razormount.h"
#include "razormountconfiguration.h"

#include <QtDebug>

#define ACT_SHOW_MENU "showMenu"
#define ACT_SHOW_INFO "showInfo"
#define ACT_NOTHING   "nothing"

EXPORT_RAZOR_PANEL_PLUGIN_CPP(RazorMount)


RazorMount::RazorMount(const RazorPanelPluginStartInfo* startInfo, QWidget* parent)
    : RazorPanelPlugin(startInfo, parent)
{
    qDebug() << "RazorMount init";
    setObjectName("RazorMount");
    m_button = new MountButton(parent, panel());
    addWidget(m_button);
}

RazorMount::~RazorMount()
{
}


void RazorMount::showConfigureDialog()
{
    RazorMountConfiguration *confWindow = this->findChild<RazorMountConfiguration*>("ClockConfigurationWindow");

    if (!confWindow)
    {
        confWindow = new RazorMountConfiguration(settings(), this);
    }

    confWindow->show();
    confWindow->raise();
    confWindow->activateWindow();
}


void RazorMount::settigsChanged()
{
    QString s = settings().value("newDeviceAction", MountButton::DevActionInfo).toString();

    if (s == ACT_SHOW_MENU)
        m_button->setDevAction(MountButton::DevActionMenu);
    else if(s == ACT_NOTHING)
        m_button->setDevAction(MountButton::DevActionNothing);
    else
        m_button->setDevAction(MountButton::DevActionInfo);
}
