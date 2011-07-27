/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * https://sourceforge.net/projects/razor-qt/
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.ru>
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


#ifndef RAZOR_MAINMENU_H
#define RAZOR_MAINMENU_H

#include "../panel/razorpanelplugin.h"
#include <qtxdg/xdgmenu.h>

#include <QPushButton>
#include <QDomElement>
#include <QAction>

#include "menustyle.h"

class QMenu;
class RazorBar;
class PowerManager;
class ScreenSaver;

class RazorMainMenu : public RazorPanelPlugin
{
    Q_OBJECT
public:
    RazorMainMenu(const RazorPanelPluginStartInfo* startInfo, QWidget* parent = 0);
    ~RazorMainMenu();

    virtual RazorPanelPlugin::Flags flags() const { return HaveConfigDialog ; }

signals:

private:
    QPushButton mButton;
    QString mLogDir;
    QString mMenuFile;
    QMenu* mMenu;
    MenuStyle mTopMenuStyle;
    MenuStyle mMenuStyle;
    PowerManager* mPowerManager;
    ScreenSaver* mScreenSaver;
    XdgMenu mXdgMenu;
    void buildMenu();

protected slots:
    virtual void showConfigureDialog();
    virtual void settigsChanged();

private slots:
    void showMenu();
};

EXPORT_RAZOR_PANEL_PLUGIN_H

#endif
