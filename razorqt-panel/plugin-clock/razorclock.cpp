#ifndef RAZORCLOCK_CPP
#define RAZORCLOCK_CPP

#include "razorclock.h"
#include "razorqt/readsettings.h"
#include <QtGui/QLabel>
#include <QtCore/QDebug>
#include <QtCore/QDateTime>
#include <QtCore/QTimer>
#include <QtGui/QCalendarWidget>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtCore/QPoint>

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

    clockFormat = settings().value("format", "hh:mm").toString();
    toolTipFormat = settings().value("tooltipFormat", "medium").toString();
    //qDebug() << "Razorclock loading";
    //gui machen
    gui = new QLabel(this);
    gui->setAlignment(Qt::AlignCenter);
    addWidget(gui);

    // ensure wi will not have moving widgets in the panel waiting for timer shot
    updateTime();

    //timer machen
    clocktimer = new QTimer(this);
    //timer mit uhrupdate verbinden
    connect (clocktimer, SIGNAL(timeout()), this, SLOT(updateTime()));
    //timer starten
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
