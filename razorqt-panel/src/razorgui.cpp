#ifndef RAZORGUI_CPP
#define RAZORGUI_CPP

#include "razor.h"
#include "razorgui.h"
//#include "razormainmenu.h"
#include "razorbar.h"
/**
 * @file razorgui.cpp
 * @brief implements class Razorgui
 * @author Christopher "VdoP" Regali
 */


/**
 * @brief constructor, initializes the parts of the razor gui
 */

RazorGUI::RazorGUI()
{
    qDebug() << "Razorgui: initializing... ";
}


/**
 * @brief adds a new Razorbar panel
 * @return the number of the created panel
 */
int RazorGUI::addPanel(const QString & configId)
{
    RazorBar* newpanel = new RazorBar(configId);
    Window wid = newpanel->effectiveWinId();
    windowList.append(wid);
    //reserve our space on the screen
    Razor::getInstance().get_Xfitman()->setStrut(wid,newpanel->height());
    Razor::getInstance().get_Xfitman()->moveWindowtoDesktop(wid,-1);
    panelList.append(newpanel);
    return panelList.count();
}

/**
 * @brief adds a widget to a Razorbar
 * @param[in] _widget a pointer to the widget to place
 * @param[in] _barnum the number of the razorbar you want to put it in
 * @param[in] _stretch the stretch-factor (see qt manual)
 * @param[in] _align the alignment (see qt manual)
 */
void RazorGUI::addWidget(RazorPlugin* _widget, int _barnum, int _stretch, Qt::Alignment _align)
{
    RazorBar* addto = panelList.at(_barnum);
    addto->addWidget(_widget,_stretch,_align);
}





/**
 * @brief destructor, deletes the razorgui
 */
RazorGUI::~RazorGUI()
{
    qDebug() << "Razorgui Dying";

}


#endif
