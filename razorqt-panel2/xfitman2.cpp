#ifndef XFITMAN_CPP
#define XFITMAN_CPP

#define dbg

#include <QtGui/QX11Info>
#include <QtCore/QList>
#include <QtGui/QApplication>
#include <QtCore/QDebug>
#include <QtGui/QDesktopWidget>

#include <stdint.h>
#include "xfitman2.h"

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xutil.h>
#include <assert.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>

/**
 * @file xfitman.cpp
 * @brief implements class Xfitman
 * @author Christopher "VdoP" Regali
 */


XfitMan2* const xfitMan2()
{
    static XfitMan2 instance;
    return &instance;
}

/**
 * @brief constructor: gets Display vars and registers us
 */
XfitMan2::XfitMan2()
{
    getAtoms();
    screen = DefaultScreen(QX11Info::display());
    root = RootWindow(QX11Info::display(),screen);
    screencount = ScreenCount(QX11Info::display());
}

/**
 * @brief moves a window to a new position
 */

void XfitMan2::moveWindow(Window _win, int _x, int _y)
{
    XMoveWindow(QX11Info::display(), _win, _x, _y);
}


/**
 * @brief this sets our background to the pixmap map
 */
void XfitMan2::setRootBackground(QPixmap _map)
{
    Pixmap p = _map.handle();
    XGrabServer(QX11Info::display());
    XChangeProperty(QX11Info::display(), root, atomMap["xrootpmap"], XA_PIXMAP, 32, PropModeReplace, (unsigned char *) &p, 1);
    XChangeProperty(QX11Info::display(), root, atomMap["esetroot"], XA_PIXMAP, 32, PropModeReplace, (unsigned char *) &p, 1);
    XSetCloseDownMode(QX11Info::display(), RetainPermanent);
    XSetWindowBackgroundPixmap(QX11Info::display(), root, p);
    XClearWindow(QX11Info::display(), root);
    XUngrabServer(QX11Info::display());
    XFlush(QX11Info::display());
}



/************************************************

 ************************************************/
bool XfitMan2::getWindowProperty(Window window,
                       Atom atom,               // property
                       Atom reqType,            // req_type
                       unsigned long* resultLen,// nitems_return
                       unsigned char** result   // prop_return
                      ) const
{
    int  format;
    unsigned long type, rest;
    return XGetWindowProperty(QX11Info::display(), window, atom, 0, 4096, false,
                              reqType, &type, &format, resultLen, &rest,
                              result)  == Success;
}



/************************************************

 ************************************************/
bool XfitMan2::getRootWindowProperty(Atom atom,    // property
                           Atom reqType,            // req_type
                           unsigned long* resultLen,// nitems_return
                           unsigned char** result   // prop_return
                          ) const
{
    return getWindowProperty(root, atom, reqType, resultLen, result);
}


/**
 * @brief this one gets the active application window.
 */
Window XfitMan2::getActiveAppWindow()
{
    Window window = getActiveWindow();

    if (acceptWindow(window))
        return window;

    Window transFor = None;
    if (XGetTransientForHint(QX11Info::display(), window, &transFor))
        return transFor;

    return 0;
}


/**
 * @brief this makes the wm send Windowevents to us which normally do not belong to zs
 */

void XfitMan2::setEventRoute()
{
    XSelectInput(QX11Info::display(), root, StructureNotifyMask | SubstructureNotifyMask);
}

/**
 * @brief returns the window that currently has inputfocus
 */
Window XfitMan2::getActiveWindow()
{
    unsigned long len;
    unsigned long *data;
    if (!getWindowProperty(root, atom("_NET_ACTIVE_WINDOW"), XA_WINDOW,
                          &len, (unsigned char**) &data)
       )
        return 0;

    Window result = 0;
    if (len)
        result = data[0];

    XFree(data);
    return result;
}


/**
 * @brief Get the number of desktops
 */

int XfitMan2::getNumDesktop()
{
    int  format;
    unsigned long type, length, rest, *data;
    XGetWindowProperty(QX11Info::display(),root,atomMap["net_number_of_desktops"],0, 4096, FALSE, XA_CARDINAL,
                       &type, &format, &length, &rest,(unsigned char**) &data);
    return data[0];
}

/**
 * @brief resizes a window to the given dimensions
 */
void XfitMan2::resizeWindow(Window _wid, int _width, int _height)
{
    XResizeWindow(QX11Info::display(), _wid, _width, _height);
}



/**
 * @brief gets a windowpixmap from a window
 */

bool XfitMan2::getClientIcon(Window _wid, QPixmap& _pixreturn)
{
    int format;
    ulong type, nitems, extra;
    ulong* data = 0;

    XGetWindowProperty(QX11Info::display(), _wid, atomMap["net_wm_icon"],
                       0, LONG_MAX, False, AnyPropertyType,
                       &type, &format, &nitems, &extra,
                       (uchar**)&data);
    if (!data)
    {
        qDebug() << "Cannot obtain pixmap info from the window";
        return false;
    }

    QImage img (data[0], data[1], QImage::Format_ARGB32);
    for (int i=0; i<img.byteCount()/4; ++i)
        ((uint*)img.bits())[i] = data[i+2];

    _pixreturn = QPixmap::fromImage(img);
    XFree(data);

    return true;
}



/**
 * @brief destructor
 */
XfitMan2::~XfitMan2()
{
}
/**
 * @brief returns a windowname and sets _nameSource to the finally used Atom
 */

//i got the idea for this from taskbar-plugin of LXPanel - so credits fly out :)
QString XfitMan2::getName(Window _wid)
{
    QString name = "";
    //first try the modern net-wm ones
    int format;
    unsigned long type, length, rest;
    unsigned char *data = NULL;
    if (XGetWindowProperty(QX11Info::display(),_wid, atomMap["net_wm_visible_name"], 0,
                           4096, FALSE, atomMap["utf8"], &type, &format, &length,
                           &rest, &data) == Success)
    {

        name = QString::fromUtf8((char*) data);
        XFree(data);

    }
    if (name.isEmpty())
        if (XGetWindowProperty(QX11Info::display(),_wid, atomMap["net_wm_name"], 0,
                               4096, FALSE, atomMap["utf8"], &type, &format, &length,
                               &rest, &data) == Success)
        {

            name = QString::fromUtf8((char*) data);
            XFree(data);
        }

    if (name.isEmpty())
        if (XGetWindowProperty(QX11Info::display(),_wid, atomMap["xa_wm_name"], 0,
                               4096, FALSE, XA_STRING, &type, &format, &length,
                               &rest, &data) == Success)
        {

            name = (char*) data;
            XFree(data);
        }

    if (name.isEmpty())
    {
        char *cName;
        Status ok = XFetchName(QX11Info::display(), _wid, &cName);
        name = QString(cName);
        if (0 != ok) XFree(cName);
    }

    if (name != "")
        return name;
    else
        return "BLAH!"; //should not happen - or is used to by getActiveAppWindow then

}

/**
 * @brief this add(1) / removes (0) / toggles (2) the _NET_WM_STATE_XXXX flag for a
 *  specified window
 * @param[in] _wid windowId for the action
 * @param[in] _atomcode the QString-code for atomMap - state
 * @param[in] _action the action to do (add 1, remove 0, toggle 2)
 */
void XfitMan2::setClientStateFlag(Window _wid, QString _atomcode, int _action)
{
    clientMessage(_wid, atomMap["net_wm_state"],_action,atomMap[_atomcode],0,0,0);
}

/**
 * @brief sends a clientmessage to a window
 */
int XfitMan2::clientMessage(Window _wid, Atom _msg,
                            unsigned long data0,
                            unsigned long data1,
                            unsigned long data2,
                            unsigned long data3,
                            unsigned long data4) const
{
    XClientMessageEvent msg;
    msg.window = _wid;
    msg.type = ClientMessage;
    msg.message_type = _msg;
    msg.send_event = true;
    msg.display = QX11Info::display();
    msg.format = 32;
    msg.data.l[0] = data0;
    msg.data.l[1] = data1;
    msg.data.l[2] = data2;
    msg.data.l[3] = data3;
    msg.data.l[4] = data4;
    if (XSendEvent(QX11Info::display(), root, FALSE, (SubstructureRedirectMask | SubstructureNotifyMask) , (XEvent *) &msg) == Success)
        return EXIT_SUCCESS;
    else
        return EXIT_FAILURE;
}

void XfitMan2::mapRaised(Window _wid)
{
    XMapRaised(QX11Info::display(), _wid);
}


/**
 * @brief returns true if a window has its hidden_flag set
 */

bool XfitMan2::isHidden(Window _wid)
{
    int  format;
    unsigned long type, length, rest, *data;
    if (XGetWindowProperty(QX11Info::display(),_wid,atomMap["net_wm_state"],0, 4096, FALSE, AnyPropertyType,
                           &type, &format, &length, &rest,(unsigned char**) &data) != Success)
        return false;

    for (unsigned int i = 0; i < length; i++)
    {
        // qDebug() << data[i] << " | " << atomMap["net_wm_state_hidden"];
        if (data[i] == atomMap["net_wm_state_hidden"])
            return true;
    }
    return false;

}

bool XfitMan2::requiresAttention(Window _wid)
{
    int  format;
    unsigned long type, length, rest, *data;
    if (XGetWindowProperty(QX11Info::display(),_wid,atomMap["net_wm_state"],0, 4096, FALSE, AnyPropertyType,
                           &type, &format, &length, &rest,(unsigned char**) &data) != Success)
        return false;

    for (unsigned int i = 0; i < length; i++)
    {
        // qDebug() << data[i] << " | " << atomMap["net_wm_state_hidden"];
        if (data[i] == atomMap["net_wm_window_demands_attention"])
            return true;
    }
    return false;
}


Atom XfitMan2::atom(const char* atomName)
{
    return XInternAtom(QX11Info::display(), atomName, false);
}


/**
 * @brief gets the used atoms into a QMap for further usage
 */
void XfitMan2::getAtoms()
{
    atomMap["net_wm_strut"] = XInternAtom(QX11Info::display(), "_NET_WM_STRUT", False);
    atomMap["net_wm_strut_partial"] = XInternAtom(QX11Info::display(), "_NET_WM_STRUT_PARTIAL", False);
    atomMap["net_client_list"] = XInternAtom(QX11Info::display(), "_NET_CLIENT_LIST", False);
    atomMap["net_wm_visible_name"] =XInternAtom(QX11Info::display(), "_NET_WM_VISIBLE_NAME", False);
    atomMap["net_wm_name"] =XInternAtom(QX11Info::display(), "_NET_WM_NAME", False);
    atomMap["xa_wm_name"] =XInternAtom(QX11Info::display(), "XA_WM_NAME", False);
    atomMap["utf8"] = XInternAtom(QX11Info::display(), "UTF8_STRING", False);
    atomMap["net_wm_icon"] = XInternAtom(QX11Info::display(), "_NET_WM_ICON", False);
    atomMap["net_wm_state_hidden"] = XInternAtom(QX11Info::display(), "_NET_WM_STATE_HIDDEN", False);
    atomMap["net_wm_state_fullscreen"] = XInternAtom(QX11Info::display(), "_NET_WM_STATE_FULLSCREEN", False);
    atomMap["net_wm_state"] = XInternAtom(QX11Info::display(), "_NET_WM_STATE", False);
    atomMap["net_current_desktop"] = XInternAtom(QX11Info::display(), "_NET_CURRENT_DESKTOP", False);
    atomMap["net_wm_desktop"] = XInternAtom(QX11Info::display(),"_NET_WM_DESKTOP", False);
    atomMap["net_active_window"] = XInternAtom(QX11Info::display(), "_NET_ACTIVE_WINDOW", False);
    atomMap["_win_workspace"] = XInternAtom(QX11Info::display(), "_WIN_WORKSPACE", False);
    atomMap["net_number_of_desktops"] = XInternAtom(QX11Info::display(), "_NET_NUMBER_OF_DESKTOPS", False);

    atomMap["net_wm_window_type"] = XInternAtom(QX11Info::display(), "_NET_WM_WINDOW_TYPE", False);
    atomMap["net_wm_window_type_normal"] = XInternAtom(QX11Info::display(), "_NET_WM_WINDOW_TYPE_NORMAL", False);
    atomMap["net_wm_window_type_desktop"] = XInternAtom(QX11Info::display(), "_NET_WM_WINDOW_TYPE_DESKTOP", False);
    atomMap["net_wm_window_type_dock"] = XInternAtom(QX11Info::display(), "_NET_WM_WINDOW_TYPE_DOCK", False);
    atomMap["net_wm_window_type_splash"] = XInternAtom(QX11Info::display(), "_NET_WM_WINDOW_TYPE_SPLASH", False);
    atomMap["kde_net_wm_window_type_override"] = XInternAtom(QX11Info::display(), "_KDE_NET_WM_WINDOW_TYPE_OVERRIDE", False);

    char atomname[20] = {0};
    qsnprintf(atomname, 20, "_NET_SYSTEM_TRAY_S%d", DefaultScreen(QX11Info::display()));
    atomMap["net_system_tray"] = XInternAtom(QX11Info::display(), atomname, False);
    atomMap["net_system_tray_opcode"] = XInternAtom(QX11Info::display(), "_NET_SYSTEM_TRAY_OPCODE", False);
    atomMap["net_manager"] = XInternAtom(QX11Info::display(), "MANAGER", False);
    atomMap["net_message_data"] = XInternAtom(QX11Info::display(), "_NET_SYSTEM_TRAY_MESSAGE_DATA", False);
    atomMap["xrootpmap"] = XInternAtom(QX11Info::display(), "_XROOTPMAP_ID", False);
    atomMap["esetroot"] = XInternAtom(QX11Info::display(), "ESETROOT_PMAP_ID", False);
    atomMap["net_wm_window_demands_attention"] = XInternAtom(QX11Info::display(), "_NET_WM_STATE_DEMANDS_ATTENTION", False);

}

AtomList XfitMan2::getWindowType(Window window) const
{
    AtomList result;

    int  format;
    unsigned long type, length, rest, *data;
    length=0;
    if (XGetWindowProperty(QX11Info::display(), window, atomMap["net_wm_window_type"],
                           0, 4096, false, AnyPropertyType, &type, &format, &length,
                           &rest, (unsigned char**) &data) != Success)
    return result;

    for (unsigned int i = 0; i < length; i++)
        result.append(data[i]);

    XFree(data);
    return result;
}


/**
 * @brief rejects a window from beeing listed
 */
bool XfitMan2::acceptWindow(Window window)
{
    {
        AtomList types = getWindowType(window);

        if (types.contains(atom("_NET_WM_WINDOW_TYPE_DESKTOP")))      return false;
        if (types.contains(atom("_NET_WM_WINDOW_TYPE_DOCK")))         return false;
        if (types.contains(atom("_NET_WM_WINDOW_TYPE_SPLASH")))       return false;
        if (types.contains(atom("_KDE_NET_WM_WINDOW_TYPE_OVERRIDE"))) return false;
    }

    Window transFor = None;
    // WM_TRANSIENT_FOR hint not set - normal window
    if (!XGetTransientForHint(QX11Info::display(), window, &transFor))
        return true;

    if (transFor == 0)      return true;
    if (transFor == window) return true;
    if (transFor == root)   return true;

     AtomList transForTypes = getWindowType(transFor);
     return !transForTypes.contains(atom("_NET_WM_WINDOW_TYPE_NORMAL"));
}



/**
 * @brief gets a client list
 */
QList<Window> XfitMan2::getClientList()
{
    //initialize the parameters for the XGetWindowProperty call
    int  format;
    unsigned long type, length, rest, *data;
    length=0;

    /**
     * @todo maybe support multiple desktops here!
     */


    QList<Window> output;

    XGetWindowProperty(QX11Info::display(),root,atomMap["net_client_list"],0, 4096, FALSE, AnyPropertyType,
                       &type, &format, &length, &rest,(unsigned char**) &data);


    for (unsigned int i = 0; i < length; i ++)
        output.append(data[i]);


    XFree(data);
    return output;
}

/**
 * @brief returns the current desktop
 */
int XfitMan2::getActiveDesktop()
{
    int  format;
    unsigned long type, length, rest, *data;
    XGetWindowProperty(QX11Info::display(),root,atomMap["net_current_desktop"],0, 4096, FALSE, XA_CARDINAL,
                       &type, &format, &length, &rest,(unsigned char**) &data);
    return data[0];
}


/**
 * @brief gets the desktop of the windows _wid
 */
int XfitMan2::getWindowDesktop(Window _wid)
{
    int  format;
    unsigned long type, length, rest, *data;
    //so we try to use net_wm_desktop first, but if the
    // system does not use net_wm standard we use win_workspace!
    if (XGetWindowProperty(QX11Info::display(),_wid,atomMap["net_wm_desktop"],0,
                           4096, FALSE, XA_CARDINAL, &type, &format, &length,
                           &rest,(unsigned char**) &data) != Success)
    {
        qDebug() << "XfitMan2::getWindowDesktop() NET_WM_DESKTOP is not set ("<<atomMap["net_wm_desktop"]<<")";
        if (XGetWindowProperty(QX11Info::display(),_wid,atomMap["_win_workspace"],0,
                               4096, FALSE, XA_CARDINAL, &type, &format, &length,
                               &rest,(unsigned char**) &data) != Success)
        {
            qDebug() << "XfitMan2::getWindowDesktop() FAILBACK: Window"
                     << _wid << "does not have set NET_WM_DESKTOP ("
                     << atomMap["net_wm_desktop"]<<") or _WIN_WORKSPACE ("
                     << atomMap["_win_workspace"]<<")";
            return -1;
        }
    }
    qDebug() << "XfitMan2::getWindowDesktop() desktop should be known";
    if (!data)
    {
        qDebug() << "XfitMan2::getWindowDesktop() WARNING *data is empty (uninitialized). And it's wrong!";
        return -1;
    }
    int desktop = data[0];
    XFree(data);
    return desktop;
}


/**
 * @brief moves a window to a specified desktop
 */

void XfitMan2::moveWindowtoDesktop(Window _wid, int _display)
{
    clientMessage(_wid,atomMap["net_wm_desktop"],(unsigned long) _display,0,0,0,0);
}

/**
 * @brief raises windows _wid
 */
void XfitMan2::raiseWindow(Window _wid)
{
    clientMessage(_wid,
                  atomMap["net_active_window"],
                  2, // source indication
                  0,
                  0,
                  0,
                  0);
    // Source indication should be 1 when the request comes from an
    // application, and 2 when it comes from a pager
}


/************************************************

 ************************************************/
void XfitMan2::minimizeWindow(Window _wid) const
{
    clientMessage(_wid, atom("WM_CHANGE_STATE"),
                  IconicState);
}


/************************************************

 ************************************************/
void XfitMan2::closeWindow(Window _wid) const
{
    clientMessage(_wid, atom("_NET_CLOSE_WINDOW"),
                  0, // Timestamp
                  2);  // Source indication should be  when it comes from a pager.
}


/**
 * @brief changes active desktop to _desktop
 */
void XfitMan2::setActiveDesktop(int _desktop)
{
    screen = _desktop;
    clientMessage(root,atomMap["net_current_desktop"],(unsigned long) _desktop,0,0,0,0);
}

/**
 * @brief this sets a window as selection owner for a specified atom - checks for success then sends the clientmessage
 */
void XfitMan2::setSelectionOwner(Window _wid, QString _selection,QString _manager)
{
    XSetSelectionOwner(QX11Info::display(), atomMap.value(_selection), _wid, CurrentTime);
    if (getSelectionOwner(_selection)== _wid)
        clientMessage(QApplication::desktop()->winId(),atomMap.value(_manager),CurrentTime,atomMap.value(_selection),_wid,0,0);
}

/**
 * @brief returns the owning window of selection _selection
 */
Window XfitMan2::getSelectionOwner(QString _selection)
{
    return XGetSelectionOwner(QX11Info::display(), atomMap.value(_selection));
}


/**
 * @brief sets net_wm_strut_partial = our reserved panelspace for the mainbar!
 */
void XfitMan2::setStrut(Window _wid,
                       int left, int right,
                       int top,  int bottom,

                       int leftStartY,   int leftEndY,
                       int rightStartY,  int rightEndY,
                       int topStartX,    int topEndX,
                       int bottomStartX, int bottomEndX
                       )
{
    qDebug() << "XfitMan2: Trying to set STRUT_PARTIAL for panel!";
    //prepare strutsize
    memset(desstrut,0,sizeof(desstrut));
    //prepare the array
    //it has format:
    /*
     * left, right, top, bottom, left_start_y, left_end_y,
     * right_start_y, right_end_y, top_start_x, top_end_x, bottom_start_x,
     * bottom_end_x
     *
     */
    //so we use bottom (index 3) bottom_start_x (index 10) and bottom_start_y (index 11)

    //so we take our panelsize from the bottom up
    desstrut[0] = left;
    desstrut[1] = right;
    desstrut[2] = top;
    desstrut[3] = bottom;

    desstrut[4] = leftStartY;
    desstrut[5] = leftEndY;

    desstrut[6] = rightStartY;
    desstrut[7] = rightEndY;

    desstrut[8] = topStartX;
    desstrut[9] = topEndX;

    desstrut[10] = bottomStartX;
    desstrut[11] = bottomEndX;

    //now we can change that property right
    XChangeProperty(QX11Info::display(), _wid , atomMap["net_wm_strut_partial"],
                    XA_CARDINAL, 32, PropModeReplace,  (unsigned char *) desstrut, 12  );

    //now some wm do not support net_wm_strut_partial but only net_wm_strut, so we also send that one too
    //xdg-std says: if you get a strut_partial ignore all following struts! so if this msg is recognized its ok
    //if not, we dont care either

    XChangeProperty(QX11Info::display(), _wid, atomMap["net_wm_strut"],
                    XA_CARDINAL, 32, PropModeReplace, (unsigned char*) desstrut, 4);
}

/**
 * @brief this unsets the strut set for panel
 */
void XfitMan2::unsetStrut(Window _wid)
{
    XDeleteProperty(QX11Info::display(), _wid, atomMap["net_wm_strut"]);
    XDeleteProperty(QX11Info::display(), _wid, atomMap["net_wm_strut_partial"]);
}


/************************************************

 ************************************************/
QString XfitMan2::debugWindow(Window wnd)
{
    if (!wnd)
        return QString("[%1]").arg(wnd,8, 16);

    QString typeStr;
    int  format;
    unsigned long type, length, rest, *data;
    length=0;
    if (XGetWindowProperty(QX11Info::display(), wnd, XInternAtom(QX11Info::display(), "_NET_WM_WINDOW_TYPE", False),
                           0, 4096, FALSE, AnyPropertyType, &type, &format,
                           &length, &rest,(unsigned char**) &data) == Success)
    {
        for (unsigned int i = 0; i < length; i++)
        {
            char* aname = XGetAtomName(QX11Info::display(), data[i]);
            typeStr = typeStr + " " + aname;
            XFree(aname);
        }
    }
    else
        typeStr ="ERROR";

    return QString("[%1] %2 %3").arg(wnd,8, 16).arg(xfitMan2()->getName(wnd)).arg(typeStr);
}

#endif
