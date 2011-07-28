/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * https://sourceforge.net/projects/razor-qt/
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
 *   Christopher Regali
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation;  only version 2 of
 * the License is valid for this program.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * END_COMMON_COPYRIGHT_HEADER */

#ifndef RAZORMODMAN_H
#define RAZORMODMAN_H

#include <QProcess>
#include <QMap>

class QDBusInterface;

typedef QMap<QString,QProcess*> ModulesMap;
typedef QMapIterator<QString,QProcess*> ModulesMapIterator;

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

    //! \brief Session configuration.
    QString mConfig;

    //! \brief map with Razor main modules. Window manager, and Razor components.
    ModulesMap procMap;

    //! \brief HAL (dbus) instance for shudown and reboot actions.
    QDBusInterface* m_power;

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
};


/*! \brief platform independent way how to set up an environment variable.
It sets env variable for all razor-session childs.
\param env a raw string variable name (PATH, TERM, ...)
\param value a QByteArray with the value. Variable will use this new value only
             - no appending/prepending is performed.
See razor_setenv_prepend.
*/
void razor_setenv(const char *env, const QByteArray &value);
/*! \brief Set up a environment variable with original value with new value prepending.
\param env a raw string with variable name
\param value a QByteArray value to be pre-pend to original content of the variable
\param separator an optional string with separator character. Eg. ":" for PATH.
See razor_setenv.
*/
void razor_setenv_prepend(const char *env, const QByteArray &value, const QByteArray &separator=":");


#endif
