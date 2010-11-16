#ifndef RAZORHANDLER_H
#define RAZORHANDLER_H

/**
 * @file razorhandler.h
 * @author Christopher "VdoP" Regali
 * @brief handles system calls by any widgets
 */

#include "defs.h"
#include "razoraboutdlg.h"

/**
 * @brief this is a simple class to just collect the slots often used in the system
 */
class RazorHandler : public QObject
{
    Q_OBJECT

public:
    RazorHandler();
    ~RazorHandler();


public slots:

    void sys_reboot();

    void sys_logout();

    void sys_shutdown();

    void gui_showabout();

private:
    QTextStream* sessionCom;


};






#endif
