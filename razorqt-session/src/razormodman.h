#ifndef RAZORMODMAN_H
#define RAZORMODMAN_H

#include <QProcess>
#include <QMap>

class QDBusInterface;

class ReadSettings;

/**
 * @file razormodman.h
 * @author Christopher "VdoP" Regali
 * @brief declares class Razormodulemanager
 */

struct Module
{
    bool power;
    QProcess * process;
};
typedef QMap<QString,Module> ModulesMap;
typedef QMapIterator<QString,Module> ModulesMapIterator;

/*! \brief RazorModuleManager manages the processes of the session
and which modules of razor are about to load.

RazorModuleManager handles the session management (logout/restart/shutdown)
as well.

Processes in RazorModuleManager are split into 2 sets.
 - basic processes, window manager, Razor components. Which has to be started
   first.
 - autostart processes, user defined 3rd party apps, which hast to be started
   *after* razor parts are running (eg. due panel's systray). Autostart is delayed
   in the RazorModuleManager constructor.

Potential process recovering is done in \see restartModules()
*/

class RazorModuleManager : public QObject
{
    Q_OBJECT

public:
    //! \brief Construct RazorModuleManager, read configuration, and start processes.
    RazorModuleManager(const QString & config, QObject * parent = 0);
    virtual ~RazorModuleManager();

public slots:
    /*! \brief Exit Razor session.
    It tries to terminate processes from procMap and autostartList
    gracefully (to kill it if it is not possible). Then the session
    exits - it returns to the kdm/gdm in most cases.
    */
    void logout();

private:
    /*! \brief Flag if the crashed process should be restarted.
    Doespower has to be set */
    bool autorestart;
    //! \brief Session configuration.
    ReadSettings* cfg;

    //! \brief map with Razor main modules. Window manager, and Razor components.
    ModulesMap procMap;
    //! \brief 3rd party processes started after main modules init.
	QList<QProcess*> autostartList;

    //! \brief HAL (dbus) instance for shudown and reboot actions.
    QDBusInterface* power;

    /*! \brief Main session management command hanlder.
    It calls one of shutdown(), reboot(), logout() methods
    regarding strings RAZOR_DO_*. See method internals.
    \todo Maybe there should be better handling of this...
    */
    void doOperation(const QString & _cmd);

    //! \brief Shutdown entire system with power d-bus interface.
    void shutdown();

    //! \brief Reboot entire system with power d-bus interface.
    void reboot();

private slots:

    /*! \brief this slot is called by the QProcesses if they end.
    \warning The slot *has* to be called as a slot only due sender() cast.

    There are 2 types of handling in this slot.

    If the process has ended correctly (no crash or kill) nothing happens
    except when the process is *not* specified with doespower in the
    configuration. If there is no "doespower" (window manager mainly)
    entire session performs logout. (Handling of the window manager 3rd
    party "logout")

    If the process crashed and is set as "doespower" it's tried to
    be restarted automatically.
    */
    void restartModules(int exitCode, QProcess::ExitStatus exitStatus);

    /*! \brief Delayed startup of autostart processes. \see autostartList
    */
	void autoStartSingleShot();

    /*! \brief Process stdout parsing.
    \warning The slot *has* to be called as a slot only due sender() cast.

    parseState() reads proces's stdout. Then it calls doOperation()
    with the single line string.
    */
    void parseState();
};


#endif
