#ifndef RAZORDESKSWITCH_CPP
#define RAZORDESKSWITCH_CPP
#include "razordeskswitch.h"
#include "razor.h"
/**
 * @file razordeskswitch.cpp
 * @author Christopher "VdoP" Regali
 * @brief implements Razordeskswitch and Razordeskswitchgui
 */

/**
 * @brief the event handler
 */
bool RazorDeskSwitch::handleEvent(XEvent* _event)
{
    if (_event->type == PropertyNotify)
    {
        desktop = Razor::getInstance().get_Xfitman()->getActiveDesktop() +1;
        if (desktop != gui->value())
            gui->setValue(desktop);
    }

    return false;
}


/**
 * @brief the constructor
 */

RazorDeskSwitch::RazorDeskSwitch(RazorBar * panel, QWidget * parent): RazorPlugin(panel, parent)
{
    //first we make th  e gui
    gui = new RazorDeskSwitchGUI(this);
    QHBoxLayout * layout = new QHBoxLayout();
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(gui);
    setLayout(layout);

    Razor::getInstance().get_events()->registerCallback(this);

}

/**
 * @brief the destructor
 */
RazorDeskSwitch::~RazorDeskSwitch()
{
    Razor::getInstance().get_events()->unregisterCallback(this);
    delete gui;
}

///**
// * @brief this switches our desktop to the given one
// */
//void RazorDeskSwitch::switchDesktop(int _newdesk)
//{
//    Razor::getInstance().get_Xfitman()->setActiveDesktop(_newdesk - 1);
//}


/**
 * @brief constructor for gui
 */
RazorDeskSwitchGUI::RazorDeskSwitchGUI(RazorDeskSwitch* parent)
    : QSpinBox(parent)
{
    //then we get an actual screen to set the gui right
    int desktop = Razor::getInstance().get_Xfitman()->getActiveDesktop() + 1;
    //now we set the gui to the appropriate values
    setValue(desktop);
    setMinimum(1);
    setMaximum(Razor::getInstance().get_Xfitman()->getNumDesktop());
    setPrefix("Desktop ");
}

/**
 * @brief manages the mousewheel-events
 */
void RazorDeskSwitchGUI::wheelEvent(QWheelEvent* _event)
{
    int numDegrees = _event->delta() / 8;
    int numSteps = numDegrees / 15;
    int oldval = value();
    if (oldval + numSteps < minimum() || oldval+numSteps > maximum())
        numSteps %= (maximum() - minimum());
    int newval = oldval + numSteps;
    setValue(newval);
//    emit changeDesk(newval);
    Razor::getInstance().get_Xfitman()->setActiveDesktop(newval - 1);
}




#endif
