/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Christopher "VdoP" Regali
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#ifndef XFITMAN_H
#define XFITMAN_H

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

// A list of atoms indicating user operations that the Window Manager supports
// for this window.
// See http://standards.freedesktop.org/wm-spec/latest/ar01s05.html#id2569373
struct WindowAllowedActions
{
    bool Move;          // indicates that the window may be moved around the screen.
    bool Resize;        // indicates that the window may be resized.
    bool Minimize;      // indicates that the window may be iconified.
    bool Shade;         // indicates that the window may be shaded.
    bool Stick;         // indicates that the window may have its sticky state toggled.
    bool MaximizeHoriz; // indicates that the window may be maximized horizontally.
    bool MaximizeVert;  // indicates that the window may be maximized vertically.
    bool FullScreen;    // indicates that the window may be brought to fullscreen state.
    bool ChangeDesktop; // indicates that the window may be moved between desktops.
    bool Close;         // indicates that the window may be closed.
    bool AboveLayer;    // indicates that the window may placed in the "above" layer of windows
    bool BelowLayer;    // indicates that the window may placed in the "below" layer of windows
};

// A list of hints describing the window state.
// http://standards.freedesktop.org/wm-spec/latest/ar01s05.html#id2569140
struct WindowState
{
    bool Modal;         // indicates that this is a modal dialog box.
    bool Sticky;        // indicates that the Window Manager SHOULD keep the window's position
                        // fixed on the screen, even when the virtual desktop scrolls.
    bool MaximizedVert; // indicates that the window is vertically maximized.
    bool MaximizedHoriz;// indicates that the window is horizontally maximized.
    bool Shaded;        // indicates that the window is shaded.
    bool SkipTaskBar;   // indicates that the window should not be included on a taskbar.
    bool SkipPager;     // indicates that the window should not be included on a Pager.
    bool Hidden;        // indicates that a window would not be visible on the screen
    bool FullScreen;    // indicates that the window should fill the entire screen.
    bool AboveLayer;    // indicates that the window should be on top of most windows.
    bool BelowLayer;    // indicates that the window should be below most windows.
    bool Attention;     // indicates that some action in or with the window happened.
};


/**
 * @brief manages the Xlib apicalls
 */
class XfitMan
{
public:

    enum Layer
    {
        LayerAbove,
        LayerNormal,
        LayerBelow
    };

    enum MaximizeDirection
    {
        MaximizeHoriz,
        MaximizeVert,
        MaximizeBoth
    };

    ~XfitMan();
    XfitMan();
    void moveWindow(Window _win, int _x, int _y) const;

    // See
    void setStrut(Window _wid,
                  int left, int right,
                  int top,  int bottom,

                  int leftStartY,   int leftEndY,
                  int rightStartY,  int rightEndY,
                  int topStartX,    int topEndX,
                  int bottomStartX, int bottomEndX
                  ) const;
    WindowList getClientList() const;
    bool getClientIcon(Window _wid, QPixmap& _pixreturn) const;
    int getWindowDesktop(Window _wid) const;
    void moveWindowToDesktop(Window _wid, int _display) const;

    void raiseWindow(Window _wid) const;
    void minimizeWindow(Window _wid) const;
    void maximizeWindow(Window _wid, MaximizeDirection direction = MaximizeBoth) const;
    void deMaximizeWindow(Window _wid) const;
    void shadeWindow(Window _wid, bool shade) const;
    void resizeWindow(Window _wid, int _width, int _height) const;
    void closeWindow(Window _wid) const;
    void setWindowLayer(Window _wid, Layer layer) const;

    void setActiveDesktop(int _desktop) const;
    bool isHidden(Window _wid) const;
    WindowAllowedActions getAllowedActions(Window window) const;
    WindowState getWindowState(Window window) const;
    int getActiveDesktop() const;
    Window getActiveAppWindow() const;
    Window getActiveWindow() const;
    int getNumDesktop() const;

    /*!
     * Returns the names of all virtual desktops. This is a list of UTF-8 encoding strings.
     *
     * Note: The number of names could be different from getNumDesktop(). If it is less
     * than getNumDesktop(), then the desktops with high numbers are unnamed. If it is
     * larger than getNumDesktop(), then the excess names outside of the getNumDesktop()
     * are considered to be reserved in case the number of desktops is increased.
     */
    QStringList getDesktopNames() const;

    /*!
     * Returns the name of virtual desktop.
     */
    QString getDesktopName(int desktopNum, const QString &defaultName=QString()) const;

    /*! Returns window title if available
     *
     */
    QString getWindowTitle(Window _wid) const;

    /*! Returns window title if available
     *
     */
    QString getApplicationName(Window _wid) const;

    bool acceptWindow(Window _wid) const;

    AtomList getWindowType(Window window) const;
#ifdef DEBUG
    static QString debugWindow(Window wnd);
#endif
    static Atom atom(const char* atomName);

    /*!
     *   QDesktopWidget have a bug http://bugreports.qt.nokia.com/browse/QTBUG-18380
     *   This workaraund this problem.
     */
    const QRect availableGeometry(int screen = -1) const;

    /*!
     *   QDesktopWidget have a bug http://bugreports.qt.nokia.com/browse/QTBUG-18380
     *   This workaraund this problem.
     */
    const QRect availableGeometry(const QWidget *widget) const;

    /*!
     *   QDesktopWidget have a bug http://bugreports.qt.nokia.com/browse/QTBUG-18380
     *   This workaraund this problem.
     */
    const QRect availableGeometry(const QPoint &point) const;

    int clientMessage(Window _wid, Atom _msg,
                      long unsigned int data0,
                      long unsigned int data1 = 0,
                      long unsigned int data2 = 0,
                      long unsigned int data3 = 0,
                      long unsigned int data4 = 0) const;

    /*!
     * Returns true if the Window Manager is running; otherwise returns false.
     */
    bool isWindowManagerActive() const;

private:

    /** \warning Do not forget to XFree(result) after data are processed!
    */
    bool getWindowProperty(Window window,
                           Atom atom,               // property
                           Atom reqType,            // req_type
                           unsigned long* resultLen,// nitems_return
                           unsigned char** result   // prop_return
                          ) const;

    /** \warning Do not forget to XFree(result) after data are processed!
    */
    bool getRootWindowProperty(Atom atom,               // property
                               Atom reqType,            // req_type
                               unsigned long* resultLen,// nitems_return
                               unsigned char** result   // prop_return
                              ) const;


    Window  root; //the actual root window on the used screen
};


const XfitMan& xfitMan();


#endif
