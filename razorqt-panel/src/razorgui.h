#ifndef RAZORGUI_H
#define RAZORGUI_H

#include "defs.h"
class RazorPlugin;
class RazorBar;

/**
 * @file razorgui.h
 * @author Chris "VdoP" Regali
 * @brief declares class Razorgui
 */


/**
 * @brief manages all the gui stuff
 */

class RazorGUI
{
public:
    RazorGUI();
    ~RazorGUI();
    int addPanel();
    void addWidget(RazorPlugin* _widget,int _barnum, int _stretch, Qt::Alignment _align);
//    /**
//     * @brief returns a list of all razor-windows to be hidden from clientlist for taskbar-reasons
//     */
//    QList<Window> getWindowList()
//    {
//        return &windowList;
//    }
    Window getWindowForPanel(RazorBar* panel)
    {
        return windowList.at(panelList.indexOf(panel));
    }
    RazorBar * panel(int index)
    {
        return panelList.at(index);
    }
private:
    QList<Window> windowList;
    QList<RazorBar*> panelList;
    QMap<int, QWidget> widgetList;
};
#endif
