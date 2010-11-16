#ifndef RAZOR_LOGMENU_H
#define RAZOR_LOGMENU_H
#include "defs.h"
#include "razorplugin.h"
/**
 * @file razorlogoutmenu.h
 * @brief declares Razorlogoutmenu
 * @author Christopher "VdoP" Regali
 */


/**
 * @brief This makes the gui
 */
class RazorLogoutMenuGUI : public QLabel
{
    Q_OBJECT
public:
    RazorLogoutMenuGUI(RazorPlugin* _owner);
    ~RazorLogoutMenuGUI();
    void makeUp();
private:
    RazorPlugin* owner;
    QAction* shutdown;

    QAction* reboot;
    QAction* logout;
    QAction* about;
    QMenu* mainMenu;
    void createMenus();
    QString icon;
    QPixmap picon;
    QString acticon;
    QPixmap actpicon;
signals:

protected:
    void mousePressEvent(QMouseEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
};

/**
 * @brief the plugin-class for the logoutmenu
 */
class RazorLogoutMenu : public RazorPlugin
{
public:
    RazorLogoutMenu(int _bar);
    ~RazorLogoutMenu();
    bool eventHandler(XEvent*);
private:
    RazorLogoutMenuGUI* gui;
};




#endif
