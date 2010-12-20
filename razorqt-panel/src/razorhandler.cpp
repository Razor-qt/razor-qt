#ifndef RAZORHANDLER_CPP
#define RAZORHANDLER_CPP

#include "razorhandler.h"

#include "razor.h"
/**
 * @brief shows the about-dlg
 */
void RazorHandler::gui_showabout()
{
    RazorAboutDLG* aboutdlg = new RazorAboutDLG();
    Q_UNUSED(aboutdlg);
}

/**
*@brief constructor sets up our handler
*/
RazorHandler::RazorHandler()
{
    sessionCom = new QTextStream(stdout);
}


/**
 * @brief Logs the user out
 */

void RazorHandler::sys_logout()
{
    *sessionCom << "RAZOR_DO_LOGOUT" << endl;
}

/**
 * @brief Reboots the system
 *
 */
void RazorHandler::sys_reboot()
{

    *sessionCom << "RAZOR_DO_REBOOT" << endl;
}

/**
 * @brief Shuts down the system
 */
void RazorHandler::sys_shutdown()
{

    *sessionCom << "RAZOR_DO_SHUTDOWN" << endl;
}


/**
 * @brief also needed for stable linking
 */
RazorHandler::~RazorHandler()
{
    delete sessionCom;
}




#endif
