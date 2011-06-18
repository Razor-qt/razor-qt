/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * https://sourceforge.net/projects/razor-qt/
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Christopher "VdoP" Regali
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


#ifndef RAZORCLOCK_CPP
#define RAZORCLOCK_CPP

#include "razorclock.h"
#include <QtGui/QLabel>
#include <QtCore/QDebug>
#include <QtCore/QDateTime>
#include <QtCore/QTimer>
#include <QtGui/QCalendarWidget>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtCore/QPoint>
#include <QtCore/QSettings>

/**
 * @file razorclock.cpp
 * @brief implements Razorclock and Razorclockgui
 * @author Christopher "VdoP" Regali
 */
EXPORT_RAZOR_PANEL_PLUGIN_CPP(RazorClock)

/**
 * @brief constructor
 */
RazorClock::RazorClock(const RazorPanelPluginStartInfo* startInfo, QWidget* parent):
        RazorPanelPlugin(startInfo, parent),
        calendarDialog(0)
{
    setObjectName("Clock");
    gui = new QLabel(this);
    gui->setAlignment(Qt::AlignCenter);
    addWidget(gui);

    settigsChanged();

    // ensure wi will not have moving widgets in the panel waiting for timer shot
    updateTime();

    clocktimer = new QTimer(this);
    connect (clocktimer, SIGNAL(timeout()), this, SLOT(updateTime()));
    clocktimer->start(1000);
}


QString getDate(QString format)
{
    if (format.compare("short", Qt::CaseInsensitive) == 0)
        return QDateTime::currentDateTime().toString(Qt::DefaultLocaleShortDate);

    if (format.compare("medium", Qt::CaseInsensitive) == 0)
        return QDateTime::currentDateTime().toString(Qt::TextDate);

    if (format.compare("long", Qt::CaseInsensitive) == 0)
        return QDateTime::currentDateTime().toString(Qt::DefaultLocaleLongDate);

    return QDateTime::currentDateTime().toString(format);
}


/**
 * @brief updates the time
 * Color and font settings can be configured in Qt CSS
 */
void RazorClock::updateTime()
{
    gui->setText(getDate(clockFormat));
    gui->setToolTip(getDate(toolTipFormat));
}


/**
 * @brief destructor
 */
RazorClock::~RazorClock()
{
    settings().setValue("format", clockFormat);
    settings().setValue("tooltipFormat", toolTipFormat);
}


void RazorClock::settigsChanged()
{
    clockFormat = settings().value("format", "hh:mm").toString();
    toolTipFormat = settings().value("tooltipFormat", "medium").toString();
}


void RazorClock::mouseReleaseEvent(QMouseEvent* event)
{
/*    if (!calendarDialog)
    {
        calendarDialog = new QDialog(this);
        //calendarDialog->setAttribute(Qt::WA_DeleteOnClose, true);
        calendarDialog->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
        calendarDialog->setLayout(new QHBoxLayout(calendarDialog));
        calendarDialog->layout()->setMargin(1);

        QCalendarWidget* cal = new QCalendarWidget(calendarDialog);
        calendarDialog->layout()->addWidget(cal);

        QPoint p;

        switch (panel()->position())
        {
            case RazorPanel::PositionTop:
                p.setX(panel()->mapToGlobal(this->geometry().topLeft()).x());
                p.setY(panel()->geometry().bottom());
                break;

        default:
                break;
        }

        calendarDialog->move(p);
        calendarDialog->show();
    }
    else
    {
        delete calendarDialog;
        calendarDialog = 0;
    }
*/
}

#endif
