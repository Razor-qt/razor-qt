#ifndef XFITMAN_CPP
#define XFITMAN_CPP

#define dbg

#include <stdint.h>
#include "xfitman.h"


/**
 * @file xfitman.cpp
 * @brief implements class Xfitman
 * @author Christopher "VdoP" Regali
 */


/**
 * @brief constructor: gets Display vars and registers us
 */
XfitMan::XfitMan()
{
    getAtoms();
    screen = DefaultScreen(QX11Info::display());
    root = RootWindow(QX11Info::display(),screen);
    screencount = ScreenCount(QX11Info::display());
}

/**
 * @brief moves a window to a new position
 */

void XfitMan::moveWindow(Window _win, int _x, int _y)
{
    XMoveWindow(QX11Info::display(), _win, _x, _y);
}


/**
 * @brief this sets our background to the pixmap map
 */
void XfitMan::setRootBackground(QPixmap _map)
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



/**
 * @brief this one gets the active application window - mainly determined by going up the tree till we find a window with title!
 */
Window XfitMan::getActiveAppWindow()
{
    int runs=0;
//    qDebug() << "Getting active Window";
    Window focuswin = getActiveWindow();
//    qDebug() << focuswin;
    QList<Window>* clientList = getClientlist();
    //while ((getName(focuswin) == "BLAH!" || !clientList.keys().contains(focuswin) ) && (focuswin != root) )
    while ((getName(focuswin) == "BLAH!" || !clientList->contains(focuswin) ) && (focuswin != root))
    {

        runs ++;
        Window parent=NULL;
        Window lroot=NULL;
        Window* childList=NULL;
        unsigned int childNum=NULL;
        XQueryTree(QX11Info::display(),focuswin,&lroot,&parent,&childList,&childNum);
        focuswin=parent;
        XFree(childList);
        //buggy windows may break this loop
        if (runs == 9)
        {
            qDebug() << "Xfitman: seems like a deadend (should not happen, happens with buggy gtk-crappy-apps though) - activating quantum-tunnel!";
            focuswin = root;
            break;
        }
    }
    //qDebug() << focuswin;
    return focuswin;

}



/**
 * @brief this makes the wm send Windowevents to us which normally do not belong to zs
 */

void XfitMan::setEventRoute()
{
    XSelectInput(QX11Info::display(), root, StructureNotifyMask | SubstructureNotifyMask);
}

/**
 * @brief returns the window that currently has inputfocus
 */
Window XfitMan::getActiveWindow()
{
    Window wid;
    int revert;
    XGetInputFocus(QX11Info::display(),&wid,&revert);
    return wid;
}


/**
 * @brief Get the number of desktops
 */

int XfitMan::getNumDesktop()
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
void XfitMan::resizeWindow(Window _wid, int _width, int _height)
{
    XResizeWindow(QX11Info::display(), _wid, _width, _height);
}



/**
 * @brief gets a windowpixmap from a window
 */

bool XfitMan::getClientIcon(Window _wid, QPixmap& _pixreturn)
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
XfitMan::~XfitMan()
{
}
/**
 * @brief returns a windowname and sets _nameSource to the finally used Atom
 */

//i got the idea for this from taskbar-plugin of LXPanel - so credits fly out :)
QString XfitMan::getName(Window _wid)
{
    QString name = "";
    //first try the modern net-wm ones
    int format;
    unsigned long type, length, rest;
    unsigned char *data = NULL;
    if (XGetWindowProperty(QX11Info::display(),_wid, atomMap["net_wm_visible_name"], 0, 4096,
                           FALSE, atomMap["utf8"], &type, &format, &length, &rest, &data) == Success)
    {

        name = QString::fromUtf8((char*) data);
        XFree(data);

    }
    if (name == "")
        if (XGetWindowProperty(QX11Info::display(),_wid, atomMap["net_wm_name"], 0, 4096,
                               FALSE, atomMap["utf8"], &type, &format, &length, &rest, &data) == Success)
        {

            name = QString::fromUtf8((char*) data);
            XFree(data);
        }

    if (name == "")
        if (XGetWindowProperty(QX11Info::display(),_wid, atomMap["xa_wm_name"], 0, 4096,
                               FALSE, XA_STRING, &type, &format, &length, &rest, &data) == Success)
        {

            name = (char*) data;
            XFree(data);
        }

    if (name != "")
        return name;
    else
        return "BLAH!"; //should not happen - or is used to by getActiveAppWindow then

}

/**
 * @brief this add(1) / removes (0) / toggles (2) the _NET_WM_STATE_XXXX flag for a specified window
 * @param[in] _wid windowId for the action
 * @param[in] _atomcode the QString-code for atomMap - state
 * @param[in] _action the action to do (add 1, remove 0, toggle 2)
 */
void XfitMan::setClientStateFlag(Window _wid, QString _atomcode, int _action)
{
    clientMessage(_wid, atomMap["net_wm_state"],_action,atomMap[_atomcode],0,0,0);
}

/**
 * @brief sends a clientmessage to a window
 */
int XfitMan::clientMessage(Window _wid, Atom _msg, unsigned long data0, unsigned long data1, unsigned long data2, unsigned long data3, unsigned long data4)
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
/**
 * @brief returns true if a window has its hidden_flag set
 */

bool XfitMan::isHidden(Window _wid)
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

bool XfitMan::requiresAttention(Window _wid)
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




/**
 * @brief gets the used atoms into a QMap for further usage
 */
void XfitMan::getAtoms()
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
    atomMap["net_wm_window_type_desktop"] = XInternAtom(QX11Info::display(), "_NET_WM_WINDOW_TYPE_DESKTOP", False);
    atomMap["net_wm_window_type_dock"] = XInternAtom(QX11Info::display(), "_NET_WM_WINDOW_TYPE_DOCK", False);
    atomMap["net_wm_window_type_splash"] = XInternAtom(QX11Info::display(), "_NET_WM_WINDOW_TYPE_SPLASH", False);
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

/**
 * @brief rejects a window from beeing listed
 */
bool XfitMan::acceptWindow(Window _wid)
{
    int  format;
    unsigned long type, length, rest, *data;
    length=0;
    if (XGetWindowProperty(QX11Info::display(),_wid,atomMap["net_wm_window_type"],0, 4096, FALSE, AnyPropertyType,
                           &type, &format, &length, &rest,(unsigned char**) &data) != Success)
        return false;
    for (unsigned int i = 0; i < length; i++)
        if (data[i] == atomMap["net_wm_window_type_desktop"] || data[i] == atomMap["net_wm_window_type_dock"] || data[i] == atomMap["net_wm_window_type_splash"])
            return false;

    return true;
}



/**
 * @brief gets a client list
 */
QList<Window>* XfitMan::getClientlist()
{
    //initialize the parameters for the XGetWindowProperty call
    int  format;
    unsigned long type, length, rest, *data;
    length=0;

    /**
     * @todo maybe support multiple desktops here!
     */


    QList<Window>* output = new QList<Window>;

    XGetWindowProperty(QX11Info::display(),root,atomMap["net_client_list"],0, 4096, FALSE, AnyPropertyType,
                       &type, &format, &length, &rest,(unsigned char**) &data);


    for (unsigned int i = 0; i < length; i ++)
        output->append(data[i]);


    XFree(data);
    return output;
}

/**
 * @brief returns the current desktop
 */
int XfitMan::getActiveDesktop()
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
int XfitMan::getWindowDesktop(Window _wid)
{
    int  format;
    unsigned long type, length, rest, *data;
    //so we try to use net_wm_desktop first, but if the system does not use net_wm standard we use win_workspace!
    if (XGetWindowProperty(QX11Info::display(),_wid,atomMap["net_wm_desktop"],0, 4096, FALSE, XA_CARDINAL,
                           &type, &format, &length, &rest,(unsigned char**) &data) != Success)
    {
        qDebug() << "XfitMan::getWindowDesktop() NET_WM_DESKTOP is not set ("<<atomMap["net_wm_desktop"]<<")";
        if (XGetWindowProperty(QX11Info::display(),_wid,atomMap["_win_workspace"],0, 4096, FALSE, XA_CARDINAL,
                           &type, &format, &length, &rest,(unsigned char**) &data) != Success)
        {
            qDebug() << "XfitMan::getWindowDesktop() FAILBACK: Window" << _wid << "does not have set NET_WM_DESKTOP ("<<atomMap["net_wm_desktop"]<<") or _WIN_WORKSPACE (" <<atomMap["_win_workspace"]<<")";
            return -1;
        }
    }
    qDebug() << "XfitMan::getWindowDesktop() desktop should be known";
    if (!data)
    {
        qDebug() << "XfitMan::getWindowDesktop() WARNING *data is empty (uninitialized). And it's wrong!";
        return -1;
    }
    int desktop = data[0];
    XFree(data);
    return desktop;
}


/**
 * @brief moves a window to a specified desktop
 */

void XfitMan::moveWindowtoDesktop(Window _wid, int _display)
{
    clientMessage(_wid,atomMap["net_wm_desktop"],(unsigned long) _display,0,0,0,0);
}

/**
 * @brief raises windows _wid
 */
void XfitMan::raiseWindow(Window _wid)
{
    //change current desktop to app_desktop if it isnt the same!
    qDebug() << "Xfitman::raiseWindow screen:" << getActiveDesktop();
    qDebug() << "Xfitman::raiseWindow windowscreen:" << getWindowDesktop(_wid);


    if (getActiveDesktop() != getWindowDesktop(_wid))
        setActiveDesktop(getWindowDesktop(_wid));

    //then tell the app its going to be active and raise it!
    isHidden(_wid);
    clientMessage(_wid,atomMap["net_active_window"],1,0,0,0,0);
    XRaiseWindow(QX11Info::display(),_wid);
}



/**
 * @brief changes active desktop to _desktop
 */
void XfitMan::setActiveDesktop(int _desktop)
{
    screen = _desktop;
    clientMessage(root,atomMap["net_current_desktop"],(unsigned long) _desktop,0,0,0,0);
}

/**
 * @brief this sets a window as selection owner for a specified atom - checks for success then sends the clientmessage
 */
void XfitMan::setSelectionOwner(Window _wid, QString _selection,QString _manager)
{
    XSetSelectionOwner(QX11Info::display(), atomMap.value(_selection), _wid, CurrentTime);
    if (getSelectionOwner(_selection)== _wid)
        clientMessage(QApplication::desktop()->winId(),atomMap.value(_manager),CurrentTime,atomMap.value(_selection),_wid,0,0);
}

/**
 * @brief returns the owning window of selection _selection
 */
Window XfitMan::getSelectionOwner(QString _selection)
{
    return XGetSelectionOwner(QX11Info::display(), atomMap.value(_selection));
}




/**
 * @brief sets net_wm_strut_partial = our reserved panelspace for the mainbar!
 */
void XfitMan::setStrut(Window _wid,int _height)
{
    qDebug() << "Xfitman: Trying to set STRUT_PARTIAL for panel!";
    //prepare strutsize
    strutsize = _height;
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
    desstrut[3]=strutsize;
    //we begin at pixel 0 left
    desstrut[10]=0;
    //and claim all the way throuth to the end
    desstrut[11]=QApplication::desktop()->width();

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
void XfitMan::unsetStrut(Window _wid)
{
    XDeleteProperty(QX11Info::display(), _wid, atomMap["net_wm_strut"]);
    XDeleteProperty(QX11Info::display(), _wid, atomMap["net_wm_strut_partial"]);
}


#endif
