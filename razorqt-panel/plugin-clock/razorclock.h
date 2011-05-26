/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * https://sourceforge.net/projects/razor-qt/
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Christopher "VdoP" Regali <
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



#ifndef RAZORCLOCK_H
#define RAZORCLOCK_H

#include "../panel/razorpanelplugin.h"

class QLabel;
class QTimer;
class QString;
class QDialog;

/**
 * @brief the clock-plugin for razorbar
 */
class RazorClock : public RazorPanelPlugin
{
    Q_OBJECT
public:
    RazorClock(const RazorPanelPluginStartInfo* startInfo, QWidget* parent = 0);
    ~RazorClock();

    virtual Alignment preferredAlignment() const { return AlignRight; }

public slots:
    void updateTime();

protected:
    void mouseReleaseEvent(QMouseEvent* event);

private:
    QTimer* clocktimer;
    QLabel * gui;
    QString clockFormat;
    QString toolTipFormat;
    QDialog* calendarDialog;
};


EXPORT_RAZOR_PANEL_PLUGIN_H

#endif
