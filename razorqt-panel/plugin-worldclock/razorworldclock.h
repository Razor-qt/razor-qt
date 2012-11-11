/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Kuzma Shapran <kuzma.shapran@gmail.com>
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

#ifndef RAZORPANELWORLDCLOCK_H
#define RAZORPANELWORLDCLOCK_H

#include "../panel/razorpanelplugin.h"
#include "razorworldclockconfiguration.h"

#include <QtGui/QLabel>


class ActiveLabel;
class QTimer;
class RazorWorldClockEventFilter;
namespace icu_48 {
    class Locale;
    class Calendar;
    class DateFormat;
}

class RazorWorldClock : public RazorPanelPlugin
{
    Q_OBJECT
public:
    RazorWorldClock(const RazorPanelPluginStartInfo* startInfo, QWidget *parent = NULL);
    ~RazorWorldClock();

    virtual RazorPanelPlugin::Flags flags() const { return PreferRightAlignment | HaveConfigDialog; }

protected slots:
    virtual void settingsChanged();
    virtual void showConfigureDialog();

private slots:
    void synchroTimeout(void);
    void timeout(void);
    void wheelScrolled(int);

private:
    ActiveLabel *mContent;

    typedef enum FormatType
    {
        FORMAT__INVALID = -1,
        FORMAT_SHORT = 0,
        FORMAT_MEDIUM,
        FORMAT_LONG,
        FORMAT_FULL,
        FORMAT_CUSTOM
    } FormatType;

    QTimer *mTimer;
    QTimer *mSynchroTimer;
    time_t mLastSynchro;

    QStringList mTimeZones;
    QString mDefaultTimeZone;
    QString mActiveTimeZone;
    QString mCustomFormat;
    FormatType mFormatType;

    icu_48::Locale *mLocale;
    icu_48::Calendar *mCalendar;
    icu_48::DateFormat *mFormat;

    void updateFormat(void);
    void updateTimezone(void);
};


class ActiveLabel : public QLabel
{
Q_OBJECT

public:
    explicit ActiveLabel(QWidget * = NULL);

signals:
    void wheelScrolled(int);

protected:
    void wheelEvent(QWheelEvent *);
};

EXPORT_RAZOR_PANEL_PLUGIN_H

#endif // RAZORPANELWORLDCLOCK_H
