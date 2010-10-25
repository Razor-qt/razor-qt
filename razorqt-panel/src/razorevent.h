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
class Razorevent : public QApplication
{
    Q_OBJECT

protected:
    virtual bool x11EventFilter(XEvent *);

public:
    Razorevent(int& _argc, char** _argv);
    ~Razorevent();
    void registerCallback(Razorplugin* _callbackClass);
    void unregisterCallback(Razorplugin* _callbackClass);
private:
    QList<Razorplugin*> callbackList;
};


#endif
