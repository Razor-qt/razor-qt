/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * https://sourceforge.net/projects/razor-qt/
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
    clockFormat = "hh:mm";
    gui = new QLabel(this);
    gui->setAlignment(Qt::AlignCenter);
    addWidget(gui);

    settigsChanged();

    clocktimer = new QTimer(this);
    connect (clocktimer, SIGNAL(timeout()), this, SLOT(updateTime()));
    clocktimer->start(1000);
}

/**
 * @brief updates the time
 * Color and font settings can be configured in Qt CSS
 */
void RazorClock::updateTime()
{
    gui->setText(QDateTime::currentDateTime().toString(clockFormat));
    gui->setToolTip(QDateTime::currentDateTime().toString(Qt::DefaultLocaleLongDate));
}

/**
 * @brief destructor
 */
RazorClock::~RazorClock()
{
}

void RazorClock::settigsChanged()
{
    QString date;

    if (QLocale::system().timeFormat(QLocale::ShortFormat).toUpper().contains("AP") == true)
    {
        clockFormat = settings().value("timeFormat", "h:mm AP").toString();
    }
    else
    {
        clockFormat = settings().value("timeFormat", "HH:mm").toString();
    }

    date = settings().value("dateFormat", Qt::SystemLocaleShortDate).toString();

    if (settings().value("showDate", false).toBool())
    {
        if (settings().value("dateOnNewLine", true).toBool())
        {
            clockFormat.append("\n");
        }
        else
        {
            clockFormat.append(" ");
        }

        clockFormat += date;
    }

    // issue #18: Panel clock plugin changes your size
    // Try to set fixed width for plugin to disallow "jumping"
    // - find minimal width for "monospaced" font.
    QFont f= font();
    f.setStyleHint(QFont::TypeWriter);

    QFontMetrics fm(f);
    QStringList splitted = clockFormat.split('\n');
    int suggestedWidth = 0;
    foreach (QString s, splitted)
    {
         int tmpWidth = fm.boundingRect(s).width();
         if (tmpWidth > suggestedWidth)
             suggestedWidth = tmpWidth;
    }
    setMaximumWidth(suggestedWidth);
    setMinimumWidth(suggestedWidth);
    // end of issue #18
    
    updateTime();
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

void RazorClock::showConfigureDialog()
{
    RazorClockConfiguration *confWindow = this->findChild<RazorClockConfiguration*>("ClockConfigurationWindow");

    if (!confWindow)
    {
        confWindow = new RazorClockConfiguration(settings(), this);
    }

    confWindow->show();
    confWindow->raise();
    confWindow->activateWindow();
}

#endif
