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
Razorclock::Razorclock(int _bar): Razorplugin(_bar)
{
    qDebug() << "Razorclock loading";
    //gui machen
    gui = new Razorclockgui(this);
    //timer machen
    clocktimer = new QTimer;
    //timer mit uhrupdate verbinden
    connect (clocktimer, SIGNAL(timeout()), this, SLOT(updateTime()));
    //timer starten
    clocktimer->start(1000);
    //gui-element hinzufügen
    Razor::getInstance().get_gui()->addWidget(gui,_bar,0,Qt::AlignRight);
}

/**
 * @brief updates the time
 * Color and font settings can be configured in Qt CSS
 */
void Razorclock::updateTime()
{
    QTime time = QTime::currentTime();
    gui->setText(QDateTime::currentDateTime().toString(
                     Razor::getInstance().get_looknfeel()->getValue("clocks_format")
                 ));
}

/**
 * @brief destructor
 */
Razorclock::~Razorclock()
{
    delete clocktimer;
    delete gui;
}

/**
 * @brief eventhandler.. not needed but needs to be implemented
 */
bool Razorclock::handleEvent(XEvent* _event)
{
    Q_UNUSED(_event);
    return false;
}

/**
 * @brief constructor
 */
Razorclockgui::Razorclockgui(Razorclock* _owner) : QLabel()
{
    owner=_owner;
    setAlignment(Qt::AlignCenter);
    show();
}



#endif
