#ifndef RAZOR_LOGMENU_H
#define RAZOR_LOGMENU_H
#include "defs.h"
#include "razorplugin.h"
/**
 * @file razorlogoutmenu.h
 * @brief declares Razorlogoutmenu
 * @author Christopher "VdoP" Regali
 */

class RazorLogoutMenuGUI;

/**
 * @brief the plugin-class for the logoutmenu
 */
class RazorLogoutMenu : public RazorPluginSquare
{
    Q_OBJECT
public:
    RazorLogoutMenu(RazorBar * panel, QWidget * parent, const QString & name);
    ~RazorLogoutMenu();
private:
    RazorLogoutMenuGUI* gui;
};

/**
 * @brief This makes the gui
 */
class RazorLogoutMenuGUI : public QLabel
{
    Q_OBJECT
public:
    RazorLogoutMenuGUI(RazorLogoutMenu * parent);
    ~RazorLogoutMenuGUI();
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

extern "C" RazorPlugin* init(RazorBar* panel, QWidget* parent, const QString & name);

#endif
