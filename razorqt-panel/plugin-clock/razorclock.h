/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Christopher "VdoP" Regali
 *   Alexander Sokoloff <sokoloff.a@gmail.ru>
 *   Maciej Płaza <plaza.maciej@gmail.com>
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
#include "razorclockconfiguration.h"
#include <QtGui/QLabel>

class QTimer;
class QString;
class QDialog;

/**
 * @brief the clock-plugin for razorbar
 */
class ClockLabel;

class RazorClock : public RazorPanelPlugin
{
    Q_OBJECT
public:
    RazorClock(const RazorPanelPluginStartInfo* startInfo, QWidget* parent = 0);
    ~RazorClock();

    virtual RazorPanelPlugin::Flags flags() const { return PreferRightAlignment | HaveConfigDialog ; }

public slots:
    void updateTime();

protected:
    virtual void mouseReleaseEvent(QMouseEvent* event);
    //virtual QSize sizeHint() const;

protected slots:
    virtual void settigsChanged();
    virtual void showConfigureDialog();

private:
    QTimer* clocktimer;
    ClockLabel * gui;
    QString clockFormat;
    QString toolTipFormat;
    QDialog* calendarDialog;
    QString timeFormat;
    QString dateFormat;
    bool dateOnNewLine;

private slots:
    void confWindowDestroyed();
    void updateMinWidth();
};


class ClockLabel: public QLabel
{
    Q_OBJECT
public:
    ClockLabel(QWidget* parent = 0):
       QLabel(parent)
    {
    }

signals:
    void fontChanged();

protected:
    bool event(QEvent *event);
};


EXPORT_RAZOR_PANEL_PLUGIN_H

#endif
