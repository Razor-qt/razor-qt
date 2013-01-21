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

#include "../panel/irazorpanelplugin.h"
#include "razorclockconfiguration.h"

#include <QtCore/QString>
#include <QtGui/QFrame>

class QLabel;
class QDialog;
class QTimer;

class RazorClock : public QObject, public IRazorPanelPlugin
{
    Q_OBJECT
public:
    RazorClock(const IRazorPanelPluginStartupInfo &startupInfo);
    ~RazorClock();

    virtual Flags flags() const { return PreferRightAlignment | HaveConfigDialog ; }
    QString themeId() const { return "Clock"; }
    QWidget *widget() { return mContent; }
    QDialog *configureDialog();
    void settingsChanged();

    void activated(ActivationReason reason);
    bool isSeparate() const { return true; }

public slots:
    void updateTime();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    QTimer* mClockTimer;
    QFrame* mContent;
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

    QDateTime currentDateTime();
    void showTime(const QDateTime &);
    void restartTimer(const QDateTime&);

private slots:
    void updateMinWidth();
};


class RazorClockPluginLibrary: public QObject, public IRazorPanelPluginLibrary
{
    Q_OBJECT
    Q_INTERFACES(IRazorPanelPluginLibrary)
public:
    IRazorPanelPlugin *instance(const IRazorPanelPluginStartupInfo &startupInfo) { return new RazorClock(startupInfo);}
};


#endif
