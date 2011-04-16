/********************************************************************

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


#ifndef RAZORTRAY_H
#define RAZORTRAY_H

#include "../panel/razorpanelplugin.h"

#include <QX11EmbedContainer>

#include <X11/X.h>
#include <X11/Xlib.h>

class TrayIcon;

/**
 * @brief This makes our trayplugin
 */

class RazorTray: public RazorPanelPlugin
{
    Q_OBJECT
public:
    RazorTray(const RazorPanelPluginStartInfo* startInfo, QWidget* parent = 0);
    ~RazorTray();

    virtual Alignment preferredAlignment() const { return AlignRight; }

signals:
    void iconSizeChanged(int iconSize);

private:
    Visual* getVisual();
    bool startTray();
    void stopTray();

    void clientMessageEvent(XClientMessageEvent* e);

    void addIcon(Window id);
    TrayIcon* findIcon(Window trayId);

    bool mValid;
    Window mTrayId;
    QList<TrayIcon*> mIcons;
    int mDamageEvent;
    int mDamageError;
private slots:
    /// This handles the events we get from the Razorplugin subsystem
    void handleEvent(XEvent* event);
};

EXPORT_RAZOR_PANEL_PLUGIN_H

#endif
