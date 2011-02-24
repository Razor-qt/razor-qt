#ifndef RAZORBARTASK_H
#define RAZORBARTASK_H


#include <QtGui/QToolButton>
#include <QtGui/QLabel>
#include <QtCore/QMap>
#include <X11/X.h>


class RazorTask;
class RazorPanel;
class RazorPanelPlugin;
class QBoxLayout;

/**
 * @file razorbartask.h
 * @author Christopher "VdoP" Regali
 * @brief declares class Razorbartask and Razorbartaskentry
 */



class RazorBarTask;

/**
 * @brief this is a single Razorbartask-entry
 */

class RazorBarTaskEntry : public QToolButton
{
    Q_OBJECT
public:
    RazorBarTaskEntry(RazorTask* _linkedtask, RazorBarTask* _owner);
    ~RazorBarTaskEntry();
    void makeUp();
    void focusGlow();

private:
    RazorBarTask* owner;
    RazorTask* linkedTask;
    QPixmap qpm;

public slots:
    void doAction(bool);
};




/**
 * @brief Is the gui-element of Razortaskmanager
 */
class RazorBarTask :public QWidget//QLabel
{
    Q_OBJECT
public:
    RazorBarTask(RazorPanelPlugin* _owner, RazorPanel * panel);
    ~RazorBarTask();
    void updateTasks(QMap<Window, RazorTask*>* _list);
    void makeUp();
    void updateFocus();
    int getEntryCount()
    {
        return taskMap.count();
   }

private:
    Window active;
    //returns a pointer to the plugin owning this gui-widget
    RazorPanelPlugin* owner;
    RazorPanel* m_panel;
    //maps a WindowId to the corresponding bartaskentry
    QMap<Window,RazorBarTaskEntry*> taskMap;
    //makes our layout
    QBoxLayout* Layout;
    QString pixm;
    QPixmap qpm;
    void addItem(QWidget* _newtaskentry);
};





#endif
