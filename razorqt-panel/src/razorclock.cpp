#ifndef RAZORCLOCK_CPP
#define RAZORCLOCK_CPP

#include "razorclock.h"
#include "razor.h"

/**
 * @file razorclock.cpp
 * @brief implements Razorclock and Razorclockgui
 * @author Christopher "VdoP" Regali
 */


/**
 * @brief constructor
 */
RazorClock::RazorClock(int _bar): RazorPlugin(_bar)
{
    qDebug() << "Razorclock loading";
    //gui machen
    gui = new RazorClockGUI(this);
    //timer machen
    clocktimer = new QTimer;
    //timer mit uhrupdate verbinden
    connect (clocktimer, SIGNAL(timeout()), this, SLOT(updateTime()));
    //timer starten
    clocktimer->start(1000);
    //gui-element hinzufügen
    Razor::getInstance().get_gui()->addWidget(gui,_bar,0,Qt::AlignRight);
    // ensure wi will not have moving widgets in the panel waiting for timer shot
    updateTime();
}

/**
 * @brief updates the time
 * Color and font settings can be configured in Qt CSS
 */
void RazorClock::updateTime()
{
    QTime time = QTime::currentTime();
    gui->setText(QDateTime::currentDateTime().toString(
                     Razor::getInstance().get_looknfeel()->getString("clocks_format")
                 ));
}

/**
 * @brief destructor
 */
RazorClock::~RazorClock()
{
    delete clocktimer;
    delete gui;
}

/**
 * @brief eventhandler.. not needed but needs to be implemented
 */
bool RazorClock::handleEvent(XEvent* _event)
{
    Q_UNUSED(_event);
    return false;
}

/**
 * @brief constructor
 */
RazorClockGUI::RazorClockGUI(RazorClock* _owner) : QLabel()
{
    owner=_owner;
    setAlignment(Qt::AlignCenter);
    show();
}



#endif
