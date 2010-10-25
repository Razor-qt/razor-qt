#ifndef RAZORTASK_H
#define RAZORTASK_H
#include "defs.h"
#include "razorplugin.h"

/**
 * @file razortask.h
 * @brief declares class Razortask and Razortaskmanager
 * @author Christopher "VdoP" Regali
 */


/**
 * @brief this is a single running task - element of the Razortaskmanager
 */


class Razorbartask;

class Razortask
{
public:
    Razortask(Window _id, int _screen);
    ~Razortask();
    Window get_client()
    {
        return client;
    }
    void setTitle(QString _title)
    {
        title = _title;
    }
    QString getTitle();
    void toogleMin();
    void setFullscreen();
    bool getIcon(QPixmap& _pixm);
    void settoDesktop(int _newdesk);
    bool isHidden();
    bool isActive();
    void raiseMe();
    int getDesktop()
    {
        return desktop;
    }
private:
    bool hidden;
    int desktop;
    bool hasIcon;
    QPixmap clientIcon;
    Window client;
    int onScreen;
    QString title;
    int state;
};

/**
 * @brief this manages the running tasks
 */

class Razortaskmanager : public Razorplugin
{
public:
    Razortaskmanager(int _bar);
    ~Razortaskmanager();
    bool handleEvent(XEvent* );

private:
    Razorbartask* gui;
    void updateMap();
    
    QMap <Window, Razortask*> clientList;
};





#endif
