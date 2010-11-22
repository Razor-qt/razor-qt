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
class RazorMainMenu : public QLabel
{
    Q_OBJECT
public:
    RazorMainMenu(RazorPluginSquare* _owner);
    ~RazorMainMenu();
    void makeUp();
private:
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
class RazorMenu : public RazorPluginSquare
{
    Q_OBJECT
public:
    RazorMenu(RazorBar * panel, QWidget * parent);
    ~RazorMenu();
private:
    RazorMainMenu* gui;
};




#endif
