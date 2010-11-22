#ifndef RAZOREVENT_H
#define RAZOREVENT_H
#include "defs.h"
#include "razorplugin.h"

/**
 * @file razorevent.h
 * @author Christopher "VdoP" Regali
 * @brief declares class Razorevent
 */

/**
 * @brief This inherits QApplication and therefore does our X11 event-handling
 */
class RazorEvent : public QApplication
{
    Q_OBJECT

protected:
    virtual bool x11EventFilter(XEvent *);

public:
    RazorEvent(int& _argc, char** _argv);
    ~RazorEvent();
    void registerCallback(RazorPlugin* _callbackClass);
    void unregisterCallback(RazorPlugin* _callbackClass);
private:
    QList<RazorPlugin*> callbackList;
};


#endif
