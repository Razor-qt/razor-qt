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
RazorClock::RazorClock(RazorBar * panel, QWidget * parent): RazorPlugin(panel, parent)
{
    qDebug() << "Razorclock loading";
    //gui machen
    gui = new QLabel(this);
    gui->setAlignment(Qt::AlignCenter);
    QHBoxLayout * layout = new QHBoxLayout();
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(gui);
    setLayout(layout);

    //timer machen
    clocktimer = new QTimer(this);
    //timer mit uhrupdate verbinden
    connect (clocktimer, SIGNAL(timeout()), this, SLOT(updateTime()));
    //timer starten
    clocktimer->start(1000);
    //gui-element hinzufügen
    //Razor::getInstance().get_gui()->addWidget(gui,_bar,0,Qt::AlignRight);
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
}

#endif
