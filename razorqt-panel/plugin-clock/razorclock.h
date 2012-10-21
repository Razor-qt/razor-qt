/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Christopher "VdoP" Regali
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *   Maciej Płaza <plaza.maciej@gmail.com>
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

#ifndef RAZORCLOCK_H
#define RAZORCLOCK_H

#include "../panel/razorpanelplugin.h"
#include "razorclockconfiguration.h"

#include <QtCore/QString>

class QLabel;
class QDialog;
class QTimer;

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

    bool event(QEvent *event);

protected slots:
    virtual void settingsChanged();
    virtual void showConfigureDialog();

private:
    QTimer* mClockTimer;
    QWidget* mContent;
    QLabel* mTimeLabel;
    QLabel* mDateLabel;
    QString mClockFormat;
    QString mToolTipFormat;
    QDialog* mCalendarDialog;
    QString mTimeFormat;
    QString mDateFormat;
    bool mDateOnNewLine;
    bool mUseUTC;
    Qt::DayOfWeek mFirstDayOfWeek;

private slots:
    void updateMinWidth();
};


EXPORT_RAZOR_PANEL_PLUGIN_H

#endif
