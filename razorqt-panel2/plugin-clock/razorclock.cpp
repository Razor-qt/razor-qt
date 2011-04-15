#ifndef RAZORCLOCK_CPP
#define RAZORCLOCK_CPP

#include "razorclock.h"
#include "razorqt/readsettings.h"
#include <QtGui/QLabel>
#include <QtCore/QDebug>
#include <QtCore/QDateTime>
#include <QtCore/QTimer>

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
        RazorPanelPlugin(startInfo, parent)
{
    setObjectName("Clock");

    clockFormat = settings().value("format", "hh:mm").toString();

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

/**
 * @brief updates the time
 * Color and font settings can be configured in Qt CSS
 */
void RazorClock::updateTime()
{
    gui->setText(QDateTime::currentDateTime().toString(clockFormat));
}

/**
 * @brief destructor
 */
RazorClock::~RazorClock()
{
}


void RazorClock::saveSettings()
{
    RazorPanelPlugin::saveSettings();
    settings().setValue("format", clockFormat);
}

#endif
