#ifndef XFITMAN_H
#define XFITMAN_H
#include "defs.h"
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xutil.h>
#include <X11/Xlib.h>
#include <assert.h>

//some net_wm state-operations we need here
#define _NET_WM_STATE_TOGGLE 2
#define _NET_WM_STATE_ADD 1
#define _NET_WM_STATE_REMOVE 0

/**
 * @file xfitman.h
 * @author Christopher "VdoP" Regali
 * @brief handles all of our xlib-calls.
 */

/**
 * @brief manages the Xlib apicalls
 */
class XfitMan
{
public:
    ~XfitMan();
    XfitMan();
    void moveWindow(Window _win, int _x, int _y);
    void setRootBackground(QPixmap _map);
    void setStrut(Window _wid, int _height);
    void unsetStrut(Window _wid);
    void getAtoms();
    QList<Window> getClientList();
    bool getClientIcon(Window _wid, QPixmap& _pixreturn);
    void setEventRoute();
    void setClientStateFlag(Window _wid, QString _atomcode, int _action);
    void setSelectionOwner(Window _wid, QString _selection, QString _manager);
    Window getSelectionOwner(QString _selection);
    int getWindowDesktop(Window _wid);
    void moveWindowtoDesktop(Window _wid, int _display);
    void raiseWindow(Window _wid);
    void resizeWindow(Window _wid, int _height, int _width);

    void setActiveDesktop(int _desktop);
    void mapRaised(Window _wid)
    {
        XMapRaised(QX11Info::display(), _wid);
    };
    bool isHidden(Window _wid);
    bool requiresAttention(Window _wid);
    int getActiveDesktop();
    Window getActiveAppWindow();
    Window getActiveWindow();
    int getNumDesktop();
    QString getName(Window _wid);
    Atom getAtom(QString _key)
    {
        return atomMap.value(_key);
    }
    bool acceptWindow(Window _wid);

private:
    int clientMessage(Window _wid, Atom _msg, long unsigned int data0, long unsigned int data1, long unsigned int data2, long unsigned int data3, long unsigned int data4);
    QString displayName;
    Window  root; //the actual root window on the used screen
    int screen; //the actual used screen
    int screencount;
    unsigned long strutsize;
    unsigned long desstrut[12];
    QMap<QString,Atom> atomMap;
};


#endif
