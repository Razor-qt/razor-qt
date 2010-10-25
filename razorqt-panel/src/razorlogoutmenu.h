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
class Razorlogoutmenugui : public QLabel
{
    Q_OBJECT
public:
    Razorlogoutmenugui(Razorplugin* _owner);
    ~Razorlogoutmenugui();
    void makeUp();
private:
    Razorplugin* owner;
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
class Razorlogoutmenu : public Razorplugin
{
public:
    Razorlogoutmenu(int _bar);
    ~Razorlogoutmenu();
    bool eventHandler(XEvent*);
private:
    Razorlogoutmenugui* gui;
};




#endif
