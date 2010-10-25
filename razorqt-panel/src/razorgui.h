#ifndef RAZORGUI_H
#define RAZORGUI_H

#include "defs.h"
#include "razorbar.h"
#include "razorbartask.h"

/**
 * @file razorgui.h
 * @author Chris "VdoP" Regali
 * @brief declares class Razorgui
 */


/**
 * @brief manages all the gui stuff
 */

class Razorgui
{
public:
    Razorgui();
    ~Razorgui();
    int addPanel();
    void addWidget(QWidget* _widget,int _barnum, int _stretch, Qt::Alignment _align);
    /**
     * @brief returns a list of all razor-windows to be hidden from clientlist for taskbar-reasons
     */
    QList<Window>* getWindowList()
    {
        return &windowList;
    }
private:
    QList<Window> windowList;
    QList<Razorbar*> panelList;
    QMap<int, QWidget> widgetList;
};
#endif
