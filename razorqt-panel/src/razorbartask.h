#ifndef RAZORBARTASK_H
#define RAZORBARTASK_H

#include "defs.h"
#include "razortask.h"
#include "razorplugin.h"

/**
 * @file razorbartask.h
 * @author Christopher "VdoP" Regali
 * @brief declares class Razorbartask and Razorbartaskentry
 */





/**
 * @brief this is a single Razorbartask-entry
 */

class Razorbartaskentry : public QToolButton
{
    Q_OBJECT
public:
    Razorbartaskentry(Razortask* _linkedtask, Razorbartask* _owner);
    ~Razorbartaskentry();
    void makeUp();


private:
    Razorbartask* owner;
    Razortask* linkedTask;
    QPixmap qpm;

public slots:
    void doAction(bool);
    //void mousePressEvent(QMouseEvent *);
    /* void enterEvent(QEvent *);
     void leaveEvent(QEvent *);*/
};




/**
 * @brief Is the gui-element of Razortaskmanager
 */
class Razorbartask :public QLabel
{
    Q_OBJECT
public:
    Razorbartask(Razorplugin* _owner);
    ~Razorbartask();
    void updateTasks(QMap<Window, Razortask*>* _list);
    void makeUp();
    void updateFocus();
    bool handleEvent(XEvent* _event);
    int getEntryCount()
    {
        return taskMap.count();
    }
private:
    Window active;
    //returns a pointer to the plugin owning this gui-widget
    Razorplugin* owner;
    //maps a WindowId to the corresponding bartaskentry
    QMap<Window,Razorbartaskentry*> taskMap;
    //makes our layout
    QHBoxLayout* Layout;
    QString pixm;
    QPixmap qpm;
    void addItem(QWidget* _newtaskentry);
};





#endif
