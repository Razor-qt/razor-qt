#ifndef RAZOR_MAINMENU_H
#define RAZOR_MAINMENU_H
#include "defs.h"
#include "razorplugin.h"
/**
 * @file razormainmenu.h
 * @brief declares Razormainmenu
 * @author Christopher "VdoP" Regali
 */


/**
 * @brief This makes the menugui - uses the xdgmanager for it
 */
class Razormainmenu : public QLabel
{
    Q_OBJECT
public:
    Razormainmenu(Razorplugin* _owner);
    ~Razormainmenu();
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
 * @brief the plugin-class for the mainmenu
 */
class Razormenu : public Razorplugin
{
public:
    Razormenu(int _bar);
    ~Razormenu();
    bool eventHandler(XEvent*);
private:
    Razormainmenu* gui;
};




#endif
