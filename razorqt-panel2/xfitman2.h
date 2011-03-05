#ifndef XFITMAN2_H
#define XFITMAN2_H

#include <QtCore/QList>
#include <QtGui/QPixmap>
#include <QtCore/QString>
#include <QtCore/QMap>
#include <X11/Xlib.h>

//some net_wm state-operations we need here
#define _NET_WM_STATE_TOGGLE 2
#define _NET_WM_STATE_ADD 1
#define _NET_WM_STATE_REMOVE 0

/**
 * @file xfitman.h
 * @author Christopher "VdoP" Regali
 * @brief handles all of our xlib-calls.
 */

typedef QList<Atom> AtomList;
typedef QList<Window> WindowList;

/**
 * @brief manages the Xlib apicalls
 */
class XfitMan2
{
public:
    ~XfitMan2();
    XfitMan2();
    void moveWindow(Window _win, int _x, int _y);
    void setRootBackground(QPixmap _map);

    // See
    void setStrut(Window _wid,
                  int left, int right,
                  int top,  int bottom,

                  int leftStartY,   int leftEndY,
                  int rightStartY,  int rightEndY,
                  int topStartX,    int topEndX,
                  int bottomStartX, int bottomEndX
                  );

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
    void minimizeWindow(Window _wid) const;
    void resizeWindow(Window _wid, int _height, int _width);
    void closeWindow(Window _wid) const;

    void setActiveDesktop(int _desktop);
    void mapRaised(Window _wid);
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

    AtomList getWindowType(Window window) const;
    static QString debugWindow(Window wnd);
    static Atom atom(const char* atomName);
private:
    int clientMessage(Window _wid, Atom _msg,
                      long unsigned int data0,
                      long unsigned int data1 = 0,
                      long unsigned int data2 = 0,
                      long unsigned int data3 = 0,
                      long unsigned int data4 = 0) const;


    bool getWindowProperty(Window window,
                           Atom atom,               // property
                           Atom reqType,            // req_type
                           unsigned long* resultLen,// nitems_return
                           unsigned char** result   // prop_return
                          ) const;

    bool getRootWindowProperty(Atom atom,               // property
                               Atom reqType,            // req_type
                               unsigned long* resultLen,// nitems_return
                               unsigned char** result   // prop_return
                              ) const;


    //QString displayName;
    Window  root; //the actual root window on the used screen
    int screen; //the actual used screen
    int screencount;
    unsigned long strutsize;
    unsigned long desstrut[12];
    QMap<QString,Atom> atomMap;
};


XfitMan2* const xfitMan2();

inline QString xEventTypeToStr(XEvent* event)
{
    switch (event->type)
    {
        case KeyPress:                return "KeyPress";
        case KeyRelease:              return "KeyRelease";
        case ButtonPress:             return "ButtonPress";
        case ButtonRelease:           return "ButtonRelease";
        case MotionNotify:            return "MotionNotify";
        case EnterNotify:             return "EnterNotify";
        case LeaveNotify:             return "LeaveNotify";
        case FocusIn:                 return "FocusIn";
        case FocusOut:                return "FocusOut";
        case KeymapNotify:            return "KeymapNotify";
        case Expose:                  return "Expose";
        case GraphicsExpose:          return "GraphicsExpose";
        case NoExpose:                return "NoExpose";
        case VisibilityNotify:        return "VisibilityNotify";
        case CreateNotify:            return "CreateNotify";
        case DestroyNotify:           return "DestroyNotify";
        case UnmapNotify:             return "UnmapNotify";
        case MapNotify:               return "MapNotify";
        case MapRequest:              return "MapRequest";
        case ReparentNotify:          return "ReparentNotify";
        case ConfigureNotify:         return "ConfigureNotify";
        case ConfigureRequest:        return "ConfigureRequest";
        case GravityNotify:           return "GravityNotify";
        case ResizeRequest:           return "ResizeRequest";
        case CirculateNotify:         return "CirculateNotify";
        case CirculateRequest:        return "CirculateRequest";
        case PropertyNotify:          return "PropertyNotify";
        case SelectionClear:          return "SelectionClear";
        case SelectionRequest:        return "SelectionRequest";
        case SelectionNotify:         return "SelectionNotify";
        case ColormapNotify:          return "ColormapNotify";
        case ClientMessage:           return "ClientMessage";
        case MappingNotify:           return "MappingNotify";
        case GenericEvent:            return "GenericEvent";
    }
    return "Unknown";
}

//#define NET_CLIENT_LIST "NET_CLIENT_LIST"
//   static const Atom NET_CLIENT_LIST_STACKING;
//    static const Atom NET_ACTIVE_WINDOW;

//    static const Atom NET_WM_WINDOW_TYPE;
//    static const Atom NET_WM_WINDOW_TYPE_NORMAL;
//    static const Atom NET_WM_WINDOW_TYPE_DOCK;
//    static const Atom NET_WM_WINDOW_TYPE_SPLASH;
//    static const Atom NET_WM_WINDOW_TYPE_DESKTOP;
//    static const Atom KDE_NET_WM_WINDOW_TYPE_OVERRIDE;


//};



#endif
