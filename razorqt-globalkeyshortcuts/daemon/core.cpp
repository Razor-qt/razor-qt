/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2013 Razor team
 * Authors:
 *   Kuzma Shapran <kuzma.shapran@gmail.com>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include <QCoreApplication>

#include <QSettings>
#include <QTimer>

#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <signal.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <stdexcept>

#include "pipe_utils.hpp"
#include "string_utils.hpp"
#include "daemon_adaptor.hpp"
#include "native_adaptor.hpp"
#include "dbus_proxy.hpp"
#include "base_action.hpp"
#include "method_action.hpp"
#include "dbus_action.hpp"
#include "command_action.hpp"

#include "core.hpp"


enum {
    X11_OP_StringToKeycode,
    X11_OP_KeycodeToString,
    X11_OP_XGrabKey,
    X11_OP_XUngrabKey,
    X11_OP_XGrabKeyboard,
    X11_OP_XUngrabKeyboard
};


static Core *s_Core = 0;


void unixSignalHandler(int signalNumber)
{
    if (s_Core)
        s_Core->unixSignalHandler(signalNumber);
}

int x11ErrorHandler(Display *display, XErrorEvent *errorEvent)
{
    if (s_Core)
        return s_Core->x11ErrorHandler(display, errorEvent);
    return 0;
}

const char * x11opcodeToString(unsigned char opcode)
{
    switch (opcode)
    {
    case X_CreateWindow:
        return "CreateWindow";
    case X_ChangeWindowAttributes:
        return "ChangeWindowAttributes";
    case X_GetWindowAttributes:
        return "GetWindowAttributes";
    case X_DestroyWindow:
        return "DestroyWindow";
    case X_DestroySubwindows:
        return "DestroySubwindows";
    case X_ChangeSaveSet:
        return "ChangeSaveSet";
    case X_ReparentWindow:
        return "ReparentWindow";
    case X_MapWindow:
        return "MapWindow";
    case X_MapSubwindows:
        return "MapSubwindows";
    case X_UnmapWindow:
        return "UnmapWindow";
    case X_UnmapSubwindows:
        return "UnmapSubwindows";
    case X_ConfigureWindow:
        return "ConfigureWindow";
    case X_CirculateWindow:
        return "CirculateWindow";
    case X_GetGeometry:
        return "GetGeometry";
    case X_QueryTree:
        return "QueryTree";
    case X_InternAtom:
        return "InternAtom";
    case X_GetAtomName:
        return "GetAtomName";
    case X_ChangeProperty:
        return "ChangeProperty";
    case X_DeleteProperty:
        return "DeleteProperty";
    case X_GetProperty:
        return "GetProperty";
    case X_ListProperties:
        return "ListProperties";
    case X_SetSelectionOwner:
        return "SetSelectionOwner";
    case X_GetSelectionOwner:
        return "GetSelectionOwner";
    case X_ConvertSelection:
        return "ConvertSelection";
    case X_SendEvent:
        return "SendEvent";
    case X_GrabPointer:
        return "GrabPointer";
    case X_UngrabPointer:
        return "UngrabPointer";
    case X_GrabButton:
        return "GrabButton";
    case X_UngrabButton:
        return "UngrabButton";
    case X_ChangeActivePointerGrab:
        return "ChangeActivePointerGrab";
    case X_GrabKeyboard:
        return "GrabKeyboard";
    case X_UngrabKeyboard:
        return "UngrabKeyboard";
    case X_GrabKey:
        return "GrabKey";
    case X_UngrabKey:
        return "UngrabKey";
    case X_AllowEvents:
        return "AllowEvents";
    case X_GrabServer:
        return "GrabServer";
    case X_UngrabServer:
        return "UngrabServer";
    case X_QueryPointer:
        return "QueryPointer";
    case X_GetMotionEvents:
        return "GetMotionEvents";
    case X_TranslateCoords:
        return "TranslateCoords";
    case X_WarpPointer:
        return "WarpPointer";
    case X_SetInputFocus:
        return "SetInputFocus";
    case X_GetInputFocus:
        return "GetInputFocus";
    case X_QueryKeymap:
        return "QueryKeymap";
    case X_OpenFont:
        return "OpenFont";
    case X_CloseFont:
        return "CloseFont";
    case X_QueryFont:
        return "QueryFont";
    case X_QueryTextExtents:
        return "QueryTextExtents";
    case X_ListFonts:
        return "ListFonts";
    case X_ListFontsWithInfo:
        return "ListFontsWithInfo";
    case X_SetFontPath:
        return "SetFontPath";
    case X_GetFontPath:
        return "GetFontPath";
    case X_CreatePixmap:
        return "CreatePixmap";
    case X_FreePixmap:
        return "FreePixmap";
    case X_CreateGC:
        return "CreateGC";
    case X_ChangeGC:
        return "ChangeGC";
    case X_CopyGC:
        return "CopyGC";
    case X_SetDashes:
        return "SetDashes";
    case X_SetClipRectangles:
        return "SetClipRectangles";
    case X_FreeGC:
        return "FreeGC";
    case X_ClearArea:
        return "ClearArea";
    case X_CopyArea:
        return "CopyArea";
    case X_CopyPlane:
        return "CopyPlane";
    case X_PolyPoint:
        return "PolyPoint";
    case X_PolyLine:
        return "PolyLine";
    case X_PolySegment:
        return "PolySegment";
    case X_PolyRectangle:
        return "PolyRectangle";
    case X_PolyArc:
        return "PolyArc";
    case X_FillPoly:
        return "FillPoly";
    case X_PolyFillRectangle:
        return "PolyFillRectangle";
    case X_PolyFillArc:
        return "PolyFillArc";
    case X_PutImage:
        return "PutImage";
    case X_GetImage:
        return "GetImage";
    case X_PolyText8:
        return "PolyText8";
    case X_PolyText16:
        return "PolyText16";
    case X_ImageText8:
        return "ImageText8";
    case X_ImageText16:
        return "ImageText16";
    case X_CreateColormap:
        return "CreateColormap";
    case X_FreeColormap:
        return "FreeColormap";
    case X_CopyColormapAndFree:
        return "CopyColormapAndFree";
    case X_InstallColormap:
        return "InstallColormap";
    case X_UninstallColormap:
        return "UninstallColormap";
    case X_ListInstalledColormaps:
        return "ListInstalledColormaps";
    case X_AllocColor:
        return "AllocColor";
    case X_AllocNamedColor:
        return "AllocNamedColor";
    case X_AllocColorCells:
        return "AllocColorCells";
    case X_AllocColorPlanes:
        return "AllocColorPlanes";
    case X_FreeColors:
        return "FreeColors";
    case X_StoreColors:
        return "StoreColors";
    case X_StoreNamedColor:
        return "StoreNamedColor";
    case X_QueryColors:
        return "QueryColors";
    case X_LookupColor:
        return "LookupColor";
    case X_CreateCursor:
        return "CreateCursor";
    case X_CreateGlyphCursor:
        return "CreateGlyphCursor";
    case X_FreeCursor:
        return "FreeCursor";
    case X_RecolorCursor:
        return "RecolorCursor";
    case X_QueryBestSize:
        return "QueryBestSize";
    case X_QueryExtension:
        return "QueryExtension";
    case X_ListExtensions:
        return "ListExtensions";
    case X_ChangeKeyboardMapping:
        return "ChangeKeyboardMapping";
    case X_GetKeyboardMapping:
        return "GetKeyboardMapping";
    case X_ChangeKeyboardControl:
        return "ChangeKeyboardControl";
    case X_GetKeyboardControl:
        return "GetKeyboardControl";
    case X_Bell:
        return "Bell";
    case X_ChangePointerControl:
        return "ChangePointerControl";
    case X_GetPointerControl:
        return "GetPointerControl";
    case X_SetScreenSaver:
        return "SetScreenSaver";
    case X_GetScreenSaver:
        return "GetScreenSaver";
    case X_ChangeHosts:
        return "ChangeHosts";
    case X_ListHosts:
        return "ListHosts";
    case X_SetAccessControl:
        return "SetAccessControl";
    case X_SetCloseDownMode:
        return "SetCloseDownMode";
    case X_KillClient:
        return "KillClient";
    case X_RotateProperties:
        return "RotateProperties";
    case X_ForceScreenSaver:
        return "ForceScreenSaver";
    case X_SetPointerMapping:
        return "SetPointerMapping";
    case X_GetPointerMapping:
        return "GetPointerMapping";
    case X_SetModifierMapping:
        return "SetModifierMapping";
    case X_GetModifierMapping:
        return "GetModifierMapping";
    case X_NoOperation:
        return "NoOperation";
    }
    return "";
}

static const char* strLevel(int level)
{
    switch (level)
    {
    case LOG_EMERG:
        return "Emergency";

    case LOG_ALERT:
        return "Alert";

    case LOG_CRIT:
        return "Critical";

    case LOG_ERR:
        return "Error";

    case LOG_WARNING:
        return "Warning";

    case LOG_NOTICE:
        return "Notice";

    case LOG_INFO:
        return "Info";

    case LOG_DEBUG:
        return "Debug";

    default:
        return "";
    }
}


Core::Core(bool useSyslog, bool minLogLevelSet, int minLogLevel, const QStringList &configFiles, bool multipleActionsBehaviourSet, MultipleActionsBehaviour multipleActionsBehaviour, QObject * parent)
    : QThread(parent)
    , LogTarget()
    , mReady(false)
    , mUseSyslog(useSyslog)
    , mMinLogLevel(minLogLevel)
    , mDisplay(0)
    , mInterClientCommunicationWindow(0)
    , mDaemonAdaptor(0)
    , mNativeAdaptor(0)
    , mDBusProxy(0)
    , mLastId(0ull)
    , mGrabbingShortcut(false)

    , AltMask(Mod1Mask)
    , MetaMask(Mod4Mask)
    , Level3Mask(Mod5Mask)
    , Level5Mask(Mod3Mask)

    , mMultipleActionsBehaviour(multipleActionsBehaviour)
    , mAllowGrabLocks(false)
    , mAllowGrabBaseSpecial(false)
    , mAllowGrabMiscSpecial(true)
    , mAllowGrabBaseKeypad(true)
    , mAllowGrabMiscKeypad(true)
    , mAllowGrabPrintable(false)

    , mSaveAllowed(false)

    , mShortcutGrabTimeout(new QTimer(this))
    , mShortcutGrabRequested(false)
{
    s_Core = this;

    initBothPipeEnds(mX11ErrorPipe);
    initBothPipeEnds(mX11RequestPipe);
    initBothPipeEnds(mX11ResponsePipe);

    mConfigFile = QString(getenv("HOME")) + "/.config/global_actions.ini";

    try
    {
        error_t c_error;

        openlog("razorqt-global-action-daemon", LOG_PID, LOG_USER);

        ::signal(SIGTERM, ::unixSignalHandler);
        ::signal(SIGINT, ::unixSignalHandler);


        if (!QDBusConnection::sessionBus().registerService("org.razorqt.global_action"))
            throw std::runtime_error(std::string("Cannot register service 'org.razorqt.global_action'"));


        if ((c_error = createPipe(mX11ErrorPipe)))
            throw std::runtime_error(std::string("Cannot create error signal pipe: ") + std::string(strerror(c_error)));

        if ((c_error = createPipe(mX11RequestPipe)))
            throw std::runtime_error(std::string("Cannot create X11 request pipe: ") + std::string(strerror(c_error)));

        if ((c_error = createPipe(mX11ResponsePipe)))
            throw std::runtime_error(std::string("Cannot create X11 response pipe: ") + std::string(strerror(c_error)));


        start();


        char signal;
        error_t error = readAll(mX11ResponsePipe[STDIN_FILENO], &signal, sizeof(signal));
        if (error > 0)
            throw std::runtime_error(std::string("Cannot read X11 start signal: ") + std::string(strerror(c_error)));
        if (error < 0)
            throw std::runtime_error(std::string("Cannot read X11 start signal"));
        if (signal)
            throw std::runtime_error(std::string("Cannot start X11 thread"));


        {
            size_t fm = configFiles.size();
            for (size_t fi = 0; fi < fm; ++fi)
            {
                mConfigFile = configFiles[fi];

                QSettings settings(mConfigFile, QSettings::IniFormat, this);

                QString iniValue;

                if (!minLogLevelSet)
                {
                    iniValue = settings.value(/* General/ */"LogLevel").toString();
                    if (!iniValue.isEmpty())
                    {
                        if (iniValue == "error")
                            mMinLogLevel = LOG_ERR;
                        else if (iniValue == "warning")
                            mMinLogLevel = LOG_WARNING;
                        else if (iniValue == "notice")
                            mMinLogLevel = LOG_NOTICE;
                        else if (iniValue == "info")
                            mMinLogLevel = LOG_INFO;
                        else if (iniValue == "debug")
                            mMinLogLevel = LOG_DEBUG;
                    }
                }

                if (!multipleActionsBehaviourSet)
                {
                    iniValue = settings.value(/* General/ */"MultipleActionsBehaviour").toString();
                    if (!iniValue.isEmpty())
                    {
                        if (iniValue == "first")
                            mMultipleActionsBehaviour = MULTIPLE_ACTIONS_BEHAVIOUR_FIRST;
                        else if (iniValue == "last")
                            mMultipleActionsBehaviour = MULTIPLE_ACTIONS_BEHAVIOUR_LAST;
                        else if (iniValue == "all")
                            mMultipleActionsBehaviour = MULTIPLE_ACTIONS_BEHAVIOUR_ALL;
                        else if (iniValue == "none")
                            mMultipleActionsBehaviour = MULTIPLE_ACTIONS_BEHAVIOUR_NONE;
                    }
                }

                mAllowGrabLocks = settings.value(/* General/ */"AllowGrabLocks", mAllowGrabLocks).toBool();
                mAllowGrabBaseSpecial = settings.value(/* General/ */"AllowGrabBaseSpecial", mAllowGrabBaseSpecial).toBool();
                mAllowGrabMiscSpecial = settings.value(/* General/ */"AllowGrabMiscSpecial", mAllowGrabMiscSpecial).toBool();
                mAllowGrabBaseKeypad = settings.value(/* General/ */"AllowGrabBaseKeypad", mAllowGrabBaseKeypad).toBool();
                mAllowGrabMiscKeypad = settings.value(/* General/ */"AllowGrabMiscKeypad", mAllowGrabMiscKeypad).toBool();
                mAllowGrabPrintable = settings.value(/* General/ */"AllowGrabPrintable", mAllowGrabPrintable).toBool();

                foreach (QString section, settings.childGroups())
                {
                    if (section != "General")
                    {
                        settings.beginGroup(section);

                        QString shortcut = section;
                        int pos = shortcut.indexOf('.');
                        if (pos != -1)
                            shortcut = shortcut.left(pos);

                        qulonglong id = 0ull;

                        bool enabled = settings.value("Enabled", true).toBool();

                        QString description = settings.value("Comment").toString();

                        if (settings.contains("Exec"))
                        {
                            QStringList values = settings.value("Exec").toStringList();
                            id = registerCommandAction(shortcut, values[0], values.mid(1), description);
                        }
                        else
                        {
                            iniValue = settings.value("DBus-service").toString();
                            if (!iniValue.isEmpty())
                            {
                                QString service = iniValue;

                                iniValue = settings.value("DBus-path").toString();
                                if (!iniValue.isEmpty())
                                {
                                    QString path = iniValue;

                                    if (settings.contains("DBus-interface"))
                                    {
                                        QString interface = settings.value("DBus-interface").toString();

                                        iniValue = settings.value("DBus-method").toString();
                                        if (!iniValue.isEmpty())
                                        {
                                            QString method = iniValue;

                                            id = registerMethodAction(shortcut, service, QDBusObjectPath(path), interface, method, description);
                                        }
                                    }
                                    else
                                    {
                                        id = registerDBusAction(shortcut, service, QDBusObjectPath(path), description);
                                    }
                                }
                            }
                        }
                        if (id)
                            mShortcutAndActionById[id].second->setEnabled(enabled);

                        settings.endGroup();
                    }
                }
            }
        }
        log(LOG_DEBUG, "Config file: %s", qPrintable(mConfigFile));


        log(LOG_DEBUG, "MinLogLevel: %s", strLevel(mMinLogLevel));
        switch (mMultipleActionsBehaviour)
        {
        case MULTIPLE_ACTIONS_BEHAVIOUR_FIRST:
            log(LOG_DEBUG, "MultipleActionsBehaviour: first");
            break;

        case MULTIPLE_ACTIONS_BEHAVIOUR_LAST:
            log(LOG_DEBUG, "MultipleActionsBehaviour: last");
            break;

        case MULTIPLE_ACTIONS_BEHAVIOUR_ALL:
            log(LOG_DEBUG, "MultipleActionsBehaviour: all");
            break;

        case MULTIPLE_ACTIONS_BEHAVIOUR_NONE:
            log(LOG_DEBUG, "MultipleActionsBehaviour: none");
            break;

        default:;
        }
        log(LOG_DEBUG, "AllowGrabLocks: %s",       mAllowGrabLocks       ? "true" : "false");
        log(LOG_DEBUG, "AllowGrabBaseSpecial: %s", mAllowGrabBaseSpecial ? "true" : "false");
        log(LOG_DEBUG, "AllowGrabMiscSpecial: %s", mAllowGrabMiscSpecial ? "true" : "false");
        log(LOG_DEBUG, "AllowGrabBaseKeypad: %s",  mAllowGrabBaseKeypad  ? "true" : "false");
        log(LOG_DEBUG, "AllowGrabMiscKeypad: %s",  mAllowGrabMiscKeypad  ? "true" : "false");
        log(LOG_DEBUG, "AllowGrabPrintable: %s",   mAllowGrabPrintable   ? "true" : "false");

        mSaveAllowed = true;
        saveConfig();



        mDaemonAdaptor = new DaemonAdaptor(this);
        if (!QDBusConnection::sessionBus().registerObject("/daemon", mDaemonAdaptor))
            throw std::runtime_error(std::string("Cannot create daemon adaptor"));

        mNativeAdaptor = new NativeAdaptor(this);
        if (!QDBusConnection::sessionBus().registerObject("/native", mNativeAdaptor))
            throw std::runtime_error(std::string("Cannot create daemon native adaptor"));

        mDBusProxy = new DBusProxy(QDBusConnection::sessionBus(), QString("org.freedesktop.DBus"), QDBusObjectPath("/org/freedesktop/DBus"), this);

        connect(mDBusProxy, SIGNAL(onServiceAppeared(QString,QString)), this, SLOT(serviceAppeared(QString,QString)));
        connect(mDBusProxy, SIGNAL(onServiceDisappeared(QString,QString)), this, SLOT(serviceDisappeared(QString,QString)));

        connect(mDaemonAdaptor, SIGNAL(onAddMethodAction(QPair<QString,qulonglong>&,QString,QString,QDBusObjectPath,QString,QString,QString)), this, SLOT(addMethodAction(QPair<QString,qulonglong>&,QString,QString,QDBusObjectPath,QString,QString,QString)));
        connect(mDaemonAdaptor, SIGNAL(onAddCommandAction(QPair<QString,qulonglong>&,QString,QString,QStringList,QString)), this, SLOT(addCommandAction(QPair<QString,qulonglong>&,QString,QString,QStringList,QString)));
        connect(mDaemonAdaptor, SIGNAL(onModifyActionDescription(bool&,qulonglong,QString)), this, SLOT(modifyActionDescription(bool&,qulonglong,QString)));
        connect(mDaemonAdaptor, SIGNAL(onModifyMethodAction(bool&,qulonglong,QString,QDBusObjectPath,QString,QString,QString)), this, SLOT(modifyMethodAction(bool&,qulonglong,QString,QDBusObjectPath,QString,QString,QString)));
        connect(mDaemonAdaptor, SIGNAL(onModifyCommandAction(bool&,qulonglong,QString,QStringList,QString)), this, SLOT(modifyCommandAction(bool&,qulonglong,QString,QStringList,QString)));
        connect(mDaemonAdaptor, SIGNAL(onEnableAction(bool&,qulonglong,bool)), this, SLOT(enableAction(bool&,qulonglong,bool)));
        connect(mDaemonAdaptor, SIGNAL(onIsActionEnabled(bool&,qulonglong)), this, SLOT(isActionEnabled(bool&,qulonglong)));
        connect(mDaemonAdaptor, SIGNAL(onChangeShortcut(QString&,qulonglong,QString)), this, SLOT(changeShortcut(QString&,qulonglong,QString)));
        connect(mDaemonAdaptor, SIGNAL(onSwapActions(bool&,qulonglong,qulonglong)), this, SLOT(swapActions(bool&,qulonglong,qulonglong)));
        connect(mDaemonAdaptor, SIGNAL(onRemoveAction(bool&,qulonglong)), this, SLOT(removeAction(bool&,qulonglong)));
        connect(mDaemonAdaptor, SIGNAL(onSetMultipleActionsBehaviour(MultipleActionsBehaviour)), this, SLOT(setMultipleActionsBehaviour(MultipleActionsBehaviour)));
        connect(mDaemonAdaptor, SIGNAL(onGetMultipleActionsBehaviour(MultipleActionsBehaviour&)), this, SLOT(getMultipleActionsBehaviour(MultipleActionsBehaviour&)));
        connect(mDaemonAdaptor, SIGNAL(onGetAllActionIds(QList<qulonglong>&)), this, SLOT(getAllActionIds(QList<qulonglong>&)));
        connect(mDaemonAdaptor, SIGNAL(onGetActionById(QPair<bool,GeneralActionInfo>&,qulonglong)), this, SLOT(getActionById(QPair<bool,GeneralActionInfo>&,qulonglong)));
        connect(mDaemonAdaptor, SIGNAL(onGetAllActions(QMap<qulonglong,GeneralActionInfo>&)), this, SLOT(getAllActions(QMap<qulonglong,GeneralActionInfo>&)));
        connect(mDaemonAdaptor, SIGNAL(onGetDBusActionInfoById(QPair<bool,DBusActionInfo>&,qulonglong)), this, SLOT(getDBusActionInfoById(QPair<bool,DBusActionInfo>&,qulonglong)));
        connect(mDaemonAdaptor, SIGNAL(onGetMethodActionInfoById(QPair<bool,MethodActionInfo>&,qulonglong)), this, SLOT(getMethodActionInfoById(QPair<bool,MethodActionInfo>&,qulonglong)));
        connect(mDaemonAdaptor, SIGNAL(onGetCommandActionInfoById(QPair<bool,CommandActionInfo>&,qulonglong)), this, SLOT(getCommandActionInfoById(QPair<bool,CommandActionInfo>&,qulonglong)));
        connect(mDaemonAdaptor, SIGNAL(onGrabShortcut(uint,QString&,bool&,bool&,bool&,QDBusMessage)), this, SLOT(grabShortcut(uint,QString&,bool&,bool&,bool&,QDBusMessage)));
        connect(mDaemonAdaptor, SIGNAL(onCancelShortcutGrab()), this, SLOT(cancelShortcutGrab()));
        connect(mDaemonAdaptor, SIGNAL(onQuit()), qApp, SLOT(quit()));

        connect(mNativeAdaptor, SIGNAL(onAddDBusAction(QPair<QString,qulonglong>&,QString,QDBusObjectPath,QString,QString)), this, SLOT(addDBusAction(QPair<QString,qulonglong>&,QString,QDBusObjectPath,QString,QString)));
        connect(mNativeAdaptor, SIGNAL(onModifyDBusAction(qulonglong&,QDBusObjectPath,QString,QString)), this, SLOT(modifyDBusAction(qulonglong&,QDBusObjectPath,QString,QString)));
        connect(mNativeAdaptor, SIGNAL(onEnableDBusAction(bool&,QDBusObjectPath,bool,QString)), this, SLOT(enableDBusAction(bool&,QDBusObjectPath,bool,QString)));
        connect(mNativeAdaptor, SIGNAL(onIsDBusActionEnabled(bool&,QDBusObjectPath,QString)), this, SLOT(isDBusActionEnabled(bool&,QDBusObjectPath,QString)));
        connect(mNativeAdaptor, SIGNAL(onChangeDBusShortcut(QPair<QString,qulonglong>&,QDBusObjectPath,QString,QString)), this, SLOT(changeDBusShortcut(QPair<QString,qulonglong>&,QDBusObjectPath,QString,QString)));
        connect(mNativeAdaptor, SIGNAL(onRemoveDBusAction(qulonglong&,QDBusObjectPath,QString)), this, SLOT(removeDBusAction(qulonglong&,QDBusObjectPath,QString)));
        connect(mNativeAdaptor, SIGNAL(onGrabShortcut(uint,QString&,bool&,bool&,bool&,QDBusMessage)), this, SLOT(grabShortcut(uint,QString&,bool&,bool&,bool&,QDBusMessage)));
        connect(mNativeAdaptor, SIGNAL(onCancelShortcutGrab()), this, SLOT(cancelShortcutGrab()));

        mShortcutGrabTimeout->setSingleShot(true);

        connect(this, SIGNAL(onShortcutGrabbed()), this, SLOT(shortcutGrabbed()), Qt::QueuedConnection);
        connect(mShortcutGrabTimeout, SIGNAL(timeout()), this, SLOT(shortcutGrabTimedout()));


        log(LOG_NOTICE, "Started");

        mReady = true;
    }
    catch (const std::exception &err)
    {
        log(LOG_CRIT, "%s", err.what());
    }
}

Core::~Core()
{
    log(LOG_INFO, "Stopping");

    closeBothPipeEnds(mX11ErrorPipe);
    closeBothPipeEnds(mX11RequestPipe);
    closeBothPipeEnds(mX11ResponsePipe);

    mX11EventLoopActive = false;
    wakeX11Thread();
    wait();

    delete mDaemonAdaptor;
    delete mDBusProxy;

    ShortcutAndActionById::iterator lastShortcutAndActionById = mShortcutAndActionById.end();
    for (ShortcutAndActionById::iterator shortcutAndActionById = mShortcutAndActionById.begin(); shortcutAndActionById != lastShortcutAndActionById; ++shortcutAndActionById)
        delete shortcutAndActionById.value().second;

    log(LOG_NOTICE, "Stopped");

    closelog();
}

void Core::saveConfig()
{
    if (!mSaveAllowed)
        return;

    QSettings settings(mConfigFile, QSettings::IniFormat);

    settings.clear();

    switch (mMultipleActionsBehaviour)
    {
    case MULTIPLE_ACTIONS_BEHAVIOUR_FIRST:
        settings.setValue(/* General/ */"MultipleActionsBehaviour", "first");
        break;

    case MULTIPLE_ACTIONS_BEHAVIOUR_LAST:
        settings.setValue(/* General/ */"MultipleActionsBehaviour", "last");
        break;

    case MULTIPLE_ACTIONS_BEHAVIOUR_ALL:
        settings.setValue(/* General/ */"MultipleActionsBehaviour", "all");
        break;

    case MULTIPLE_ACTIONS_BEHAVIOUR_NONE:
        settings.setValue(/* General/ */"MultipleActionsBehaviour", "none");
        break;

    default:;
    }

    settings.setValue(/* General/ */"AllowGrabLocks",       mAllowGrabLocks      );
    settings.setValue(/* General/ */"AllowGrabBaseSpecial", mAllowGrabBaseSpecial);
    settings.setValue(/* General/ */"AllowGrabMiscSpecial", mAllowGrabMiscSpecial);
    settings.setValue(/* General/ */"AllowGrabBaseKeypad",  mAllowGrabBaseKeypad );
    settings.setValue(/* General/ */"AllowGrabMiscKeypad",  mAllowGrabMiscKeypad );
    settings.setValue(/* General/ */"AllowGrabPrintable",   mAllowGrabPrintable  );

    ShortcutAndActionById::const_iterator lastShortcutAndActionById = mShortcutAndActionById.end();
    for (ShortcutAndActionById::const_iterator shortcutAndActionById = mShortcutAndActionById.begin(); shortcutAndActionById != lastShortcutAndActionById; ++shortcutAndActionById)
    {
        const BaseAction *action = shortcutAndActionById.value().second;
        QString section = shortcutAndActionById.value().first + "." + QString::number(shortcutAndActionById.key());

        bool persistent = false;
        if (!strcmp(action->type(), CommandAction::id()))
        {
            persistent = true;
        }
        else if (!strcmp(action->type(), MethodAction::id()))
        {
            persistent = true;
        }
        else if (!strcmp(action->type(), DBusAction::id()))
        {
            const DBusAction *dBusAction = dynamic_cast<const DBusAction*>(action);
            persistent = dBusAction->isPersistent();
        }

        if (persistent)
        {
            settings.beginGroup(section);

            settings.setValue("Enabled", action->isEnabled());
            settings.setValue("Comment", action->description());

            if (!strcmp(action->type(), CommandAction::id()))
            {
                const CommandAction *commandAction = dynamic_cast<const CommandAction*>(action);
                settings.setValue("Exec", QVariant(QStringList() << commandAction->command() += commandAction->args()));
            }
            else if (!strcmp(action->type(), MethodAction::id()))
            {
                const MethodAction *methodAction = dynamic_cast<const MethodAction*>(action);
                settings.setValue("DBus-service",   methodAction->service());
                settings.setValue("DBus-path",      methodAction->path().path());
                settings.setValue("DBus-interface", methodAction->interface());
                settings.setValue("DBus-method",    methodAction->method());
            }
            else if (!strcmp(action->type(), DBusAction::id()))
            {
                const DBusAction *dBusAction = dynamic_cast<const DBusAction*>(action);
                settings.setValue("DBus-service", dBusAction->service());
                settings.setValue("DBus-path",    dBusAction->path().path());
            }

            settings.endGroup();
        }
    }
}

void Core::unixSignalHandler(int signalNumber)
{
    log(LOG_INFO, "Signal #%d received", signalNumber);
    qApp->quit();
}

void Core::log(int level, const char *format, ...) const
{
    if (level > mMinLogLevel)
        return;

    va_list ap;
    va_start(ap, format);
    if (mUseSyslog)
        vsyslog(LOG_MAKEPRI(LOG_USER, level), format, ap);
    else
    {
        fprintf(stderr, "[%s] ", strLevel(level));
        vfprintf(stderr, format, ap);
        fprintf(stderr, "\n");
    }
    va_end(ap);
}

int Core::x11ErrorHandler(Display */*display*/, XErrorEvent *errorEvent)
{
    if (error_t error = writeAll(mX11ErrorPipe[STDOUT_FILENO], errorEvent, sizeof(XErrorEvent)))
    {
        log(LOG_CRIT, "Cannot write to error signal pipe: %s", strerror(error));
        qApp->quit();
        return 0;
    }

    return 0;
}

bool Core::waitForX11Error(int level, uint timeout)
{
    pollfd *fds = new pollfd[1];
    fds[0].fd = mX11ErrorPipe[STDIN_FILENO];
    fds[0].events = POLLIN | POLLERR | POLLHUP;
    if (poll(fds, 1, timeout) < 0)
        return true;

    bool result = false;

    while (fds[0].revents & POLLIN)
    {
        XErrorEvent errorEvent;
        if (error_t error = readAll(mX11ErrorPipe[STDIN_FILENO], &errorEvent, sizeof(errorEvent)))
        {
            log(LOG_CRIT, "Cannot read from error signal pipe: %s", strerror(error));
            qApp->quit();
        }

        char errorString[1024];
        XGetErrorText(errorEvent.display, errorEvent.error_code, errorString, 1023);
        log(level, "X11 error: type: %d, serial: %lu, error_code: %d '%s', request_code: %d (%s), minor_code: %d, resourceid: %lu", errorEvent.type, errorEvent.serial, errorEvent.error_code, errorString, errorEvent.request_code, x11opcodeToString(errorEvent.request_code), errorEvent.minor_code, errorEvent.resourceid);

        result = true;

        if (poll(fds, 1, 0) < 0)
            return true;
    }

    return result;
}

void Core::lockX11Error()
{
    mX11ErrorMutex.lock();
    waitForX11Error(false, 0);
}

bool Core::checkX11Error(int level, uint timeout)
{
//    unsigned long serial = NextRequest(mDisplay);
//    log(LOG_DEBUG, "X11 error: serial: %lu", serial);

    bool result = waitForX11Error(level, timeout);
    mX11ErrorMutex.unlock();
    return result;
}

void Core::wakeX11Thread()
{
    if (mInterClientCommunicationWindow)
    {
        XClientMessageEvent dummyEvent;
        memset(&dummyEvent, 0, sizeof(dummyEvent));
        dummyEvent.type = ClientMessage;
        dummyEvent.window = mInterClientCommunicationWindow;
        dummyEvent.format = 32;

        lockX11Error();
        XSendEvent(mDisplay, mInterClientCommunicationWindow, 0, 0, reinterpret_cast<XEvent*>(&dummyEvent));
        checkX11Error();
        XFlush(mDisplay);
    }
}

bool Core::isEscape(KeySym keySym, unsigned int modifiers)
{
    return ((keySym == XK_Escape) && (!modifiers));
}

bool Core::isModifier(KeySym keySym)
{
    switch (keySym)
    {
    case XK_Shift_L:
    case XK_Shift_R:
    case XK_Control_L:
    case XK_Control_R:
    case XK_Meta_L:
    case XK_Meta_R:
    case XK_Alt_L:
    case XK_Alt_R:
    case XK_Super_L:
    case XK_Super_R:
    case XK_Hyper_L:
    case XK_Hyper_R:
    case XK_ISO_Level3_Shift:
    case XK_ISO_Level5_Shift:
    case XK_ISO_Group_Shift:
        return true;

    }
    return false;
}

bool Core::isAllowed(KeySym keySym, unsigned int modifiers)
{
    switch (keySym)
    {
    case XK_Scroll_Lock:
    case XK_Num_Lock:
    case XK_Caps_Lock:
    case XK_ISO_Lock:
    case XK_ISO_Level3_Lock:
    case XK_ISO_Level5_Lock:
    case XK_ISO_Group_Lock:
    case XK_ISO_Next_Group_Lock:
    case XK_ISO_Prev_Group_Lock:
    case XK_ISO_First_Group_Lock:
    case XK_ISO_Last_Group_Lock:
        if (!modifiers)
            return mAllowGrabLocks;
        break;

    case XK_Home:
    case XK_Left:
    case XK_Up:
    case XK_Right:
    case XK_Down:
    case XK_Page_Up:
    case XK_Page_Down:
    case XK_End:
    case XK_Delete:
    case XK_Insert:
    case XK_BackSpace:
    case XK_Tab:
    case XK_Return:
    case XK_space:
        if (!modifiers)
            return mAllowGrabBaseSpecial;
        break;

    case XK_Pause:
    case XK_Print:
    case XK_Linefeed:
    case XK_Clear:
    case XK_Multi_key:
    case XK_Codeinput:
    case XK_SingleCandidate:
    case XK_MultipleCandidate:
    case XK_PreviousCandidate:
    case XK_Begin:
    case XK_Select:
    case XK_Execute:
    case XK_Undo:
    case XK_Redo:
    case XK_Menu:
    case XK_Find:
    case XK_Cancel:
    case XK_Help:
    case XK_Sys_Req:
    case XK_Break:
        if (!modifiers)
            return mAllowGrabMiscSpecial;
        break;

    case XK_KP_Enter:
    case XK_KP_Home:
    case XK_KP_Left:
    case XK_KP_Up:
    case XK_KP_Right:
    case XK_KP_Down:
    case XK_KP_Page_Up:
    case XK_KP_Page_Down:
    case XK_KP_End:
    case XK_KP_Begin:
    case XK_KP_Insert:
    case XK_KP_Delete:
    case XK_KP_Multiply:
    case XK_KP_Add:
    case XK_KP_Subtract:
    case XK_KP_Decimal:
    case XK_KP_Divide:
    case XK_KP_0:
    case XK_KP_1:
    case XK_KP_2:
    case XK_KP_3:
    case XK_KP_4:
    case XK_KP_5:
    case XK_KP_6:
    case XK_KP_7:
    case XK_KP_8:
    case XK_KP_9:
        if (!modifiers)
            return mAllowGrabBaseKeypad;
        break;

    case XK_KP_Space:
    case XK_KP_Tab:
    case XK_KP_F1:
    case XK_KP_F2:
    case XK_KP_F3:
    case XK_KP_F4:
    case XK_KP_Equal:
    case XK_KP_Separator:
        if (!modifiers)
            return mAllowGrabMiscKeypad;
        break;

    case XK_grave:
    case XK_1:
    case XK_2:
    case XK_3:
    case XK_4:
    case XK_5:
    case XK_6:
    case XK_7:
    case XK_8:
    case XK_9:
    case XK_0:
    case XK_minus:
    case XK_equal:
    case XK_Q:
    case XK_W:
    case XK_E:
    case XK_R:
    case XK_T:
    case XK_Y:
    case XK_U:
    case XK_I:
    case XK_O:
    case XK_P:
    case XK_bracketleft:
    case XK_bracketright:
    case XK_backslash:
    case XK_A:
    case XK_S:
    case XK_D:
    case XK_F:
    case XK_G:
    case XK_H:
    case XK_J:
    case XK_K:
    case XK_L:
    case XK_semicolon:
    case XK_apostrophe:
    case XK_Z:
    case XK_X:
    case XK_C:
    case XK_V:
    case XK_B:
    case XK_N:
    case XK_M:
    case XK_comma:
    case XK_period:
    case XK_slash:
        if (!(modifiers & ~(ShiftMask | Level3Mask | Level5Mask)))
            return mAllowGrabPrintable;
        break;

    }
    return true;
}

void Core::run()
{
    mX11EventLoopActive = true;

    XInitThreads();

    int (*oldx11ErrorHandler)(Display *display, XErrorEvent *errorEvent) = XSetErrorHandler(::x11ErrorHandler);

    mDisplay = XOpenDisplay(NULL);
    XSynchronize(mDisplay, True);

    lockX11Error();

    Window rootWindow = DefaultRootWindow(mDisplay);

    XSelectInput(mDisplay, rootWindow, KeyPressMask);

    mInterClientCommunicationWindow = XCreateSimpleWindow(mDisplay, rootWindow, 0, 0, 1, 1, 0, 0, 0);

    XSelectInput(mDisplay, mInterClientCommunicationWindow, StructureNotifyMask);

    if (checkX11Error())
        return;

    QSet<unsigned int> allModifiers;
    unsigned int allShifts = ShiftMask | ControlMask | AltMask | MetaMask | Level3Mask | Level5Mask;
    unsigned int ignoreMask = 0xff ^ allShifts;
    for (unsigned int i = 0; i < 0x100; ++i)
    {
        unsigned int ignoreLocks = i & ignoreMask;
        allModifiers.insert(ignoreLocks);
    }


    char signal = 0;
    if (write(mX11ResponsePipe[STDOUT_FILENO], &signal, sizeof(signal)) == sizeof(signal))
    {
        XEvent event;
        while (mX11EventLoopActive)
        {
            XNextEvent(mDisplay, &event);
            if (!mX11EventLoopActive)
                break;

            switch (event.type)
            {
            case KeyPress:
            {
                QMutexLocker lock(&mDataMutex);

                if (mGrabbingShortcut)
                {
//                    log(LOG_DEBUG, "KeyPress %08x %08x", event.xkey.state, event.xkey.keycode);

                    bool ignoreKey = false;
                    bool cancel = false;
                    QString shortcut;

                    int keysymsPerKeycode;
                    lockX11Error();
                    KeySym *keySyms = XGetKeyboardMapping(mDisplay, event.xkey.keycode, 1, &keysymsPerKeycode);
                    checkX11Error();

                    if (keysymsPerKeycode)
                    {
                        if (keySyms[0])
                        {
                            KeySym keySym = 0;

//                            log(LOG_DEBUG, "keysymsPerKeycode %d", keysymsPerKeycode);

//                            for (int i = 0; i < keysymsPerKeycode; ++i)
//                                log(LOG_DEBUG, "keySym #%d %08x", i, keySyms[i]);

                            if ((keysymsPerKeycode >= 2) && keySyms[1] && (keySyms[0] >= XK_a) && (keySyms[0] <= XK_z))
                                keySym = keySyms[1];
                            else if (keysymsPerKeycode >= 1)
                                keySym = keySyms[0];

                            if (keySym)
                            {
                                if (isEscape(keySym, event.xkey.state & allShifts))
                                    cancel = true;
                                else
                                {
                                    if (isModifier(keySym) || !isAllowed(keySym, event.xkey.state & allShifts))
                                        ignoreKey = true;
                                    else
                                    {
                                        char *str = XKeysymToString(keySym);

                                        if (str && *str)
                                        {
                                            if (event.xkey.state & ShiftMask)
                                                shortcut += "Shift+";
                                            if (event.xkey.state & ControlMask)
                                                shortcut += "Control+";
                                            if (event.xkey.state & AltMask)
                                                shortcut += "Alt+";
                                            if (event.xkey.state & MetaMask)
                                                shortcut += "Meta+";
                                            if (event.xkey.state & Level3Mask)
                                                shortcut += "Level3+";
                                            if (event.xkey.state & Level5Mask)
                                                shortcut += "Level5+";

                                            shortcut += str;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    if (!ignoreKey)
                    {
                        IdsByShortcut::iterator idsByShortcut = mIdsByShortcut.find(shortcut);
                        if ((idsByShortcut == mIdsByShortcut.end()) || (idsByShortcut.value().isEmpty()))
                        {
                            log(LOG_DEBUG, "grabShortcut: checking %s", qPrintable(shortcut));
                            lockX11Error();
                            XUngrabKeyboard(mDisplay, CurrentTime);
                            checkX11Error();

                            QSet<unsigned int>::const_iterator lastAllModifiers = allModifiers.end();
                            for (QSet<unsigned int>::const_iterator modifiers = allModifiers.begin(); modifiers != lastAllModifiers; ++modifiers)
                            {
                                log(LOG_DEBUG, "grabShortcut: checking %02x + %02x", event.xkey.keycode, event.xkey.state | *modifiers);
                                lockX11Error();
                                XGrabKey(mDisplay, event.xkey.keycode, event.xkey.state | *modifiers, rootWindow, False, GrabModeAsync, GrabModeAsync);
                                ignoreKey |= checkX11Error(LOG_DEBUG);
                            }

                            lockX11Error();
                            XUngrabKey(mDisplay, event.xkey.keycode, event.xkey.state, rootWindow);
                            checkX11Error();

                            if (ignoreKey)
                            {
                                lockX11Error();
                                XGrabKeyboard(mDisplay, rootWindow, False, GrabModeAsync, GrabModeAsync, CurrentTime);
                                checkX11Error();
                            }
                        }
                    }
                    if (!ignoreKey)
                    {
                        mGrabbingShortcut = false;

                        if (error_t error = writeAll(mX11ResponsePipe[STDOUT_FILENO], &cancel, sizeof(cancel)))
                        {
                            log(LOG_CRIT, "Cannot write to X11 response pipe: %s", strerror(error));
                            close(mX11RequestPipe[STDIN_FILENO]);
                            mX11EventLoopActive = false;
                            break;
                        }
                        if (!cancel)
                        {
                            size_t length = shortcut.length();
                            if (error_t error = writeAll(mX11ResponsePipe[STDOUT_FILENO], &length, sizeof(length)))
                            {
                                log(LOG_CRIT, "Cannot write to X11 response pipe: %s", strerror(error));
                                close(mX11RequestPipe[STDIN_FILENO]);
                                mX11EventLoopActive = false;
                                break;
                            }
                            if (error_t error = writeAll(mX11ResponsePipe[STDOUT_FILENO], qPrintable(shortcut), length))
                            {
                                log(LOG_CRIT, "Cannot write to X11 response pipe: %s", strerror(error));
                                close(mX11RequestPipe[STDIN_FILENO]);
                                mX11EventLoopActive = false;
                                break;
                            }
                        }

                        emit onShortcutGrabbed();
                    }
                }
                else
                {
                    QString shortcut = mShortcutByX11[qMakePair(static_cast<KeyCode>(event.xkey.keycode), event.xkey.state & allShifts)];
                    log(LOG_DEBUG, "KeyPress %08x %08x %s", event.xkey.state & allShifts, event.xkey.keycode, qPrintable(shortcut));

                    IdsByShortcut::iterator idsByShortcut = mIdsByShortcut.find(shortcut);
                    if (idsByShortcut != mIdsByShortcut.end())
                    {
                        Ids &ids = idsByShortcut.value();
                        switch (mMultipleActionsBehaviour)
                        {
                        case MULTIPLE_ACTIONS_BEHAVIOUR_FIRST:
                        {
                            Ids::iterator lastIds = ids.end();
                            for (Ids::iterator idi = ids.begin(); idi != lastIds; ++idi)
                                if (mShortcutAndActionById[*idi].second->call())
                                    break;
                        }
                            break;

                        case MULTIPLE_ACTIONS_BEHAVIOUR_LAST:
                        {
                            Ids::iterator firstIds = ids.begin();
                            for (Ids::iterator idi = ids.end(); idi != firstIds; )
                            {
                                --idi;
                                if (mShortcutAndActionById[*idi].second->call())
                                    break;
                            }
                        }
                            break;

                        case MULTIPLE_ACTIONS_BEHAVIOUR_NONE:
                            if (ids.size() == 1)
                                mShortcutAndActionById[*(ids.begin())].second->call();
                            break;

                        case MULTIPLE_ACTIONS_BEHAVIOUR_ALL:
                        {
                            Ids::iterator lastIds = ids.end();
                            for (Ids::iterator idi = ids.begin(); idi != lastIds; ++idi)
                                mShortcutAndActionById[*idi].second->call();
                        }
                            break;

                        default:;
                        }
                    }
                }
            }
                break;

            default:
                {
                    pollfd *fds = new pollfd[1];
                    fds[0].fd = mX11RequestPipe[STDIN_FILENO];
                    fds[0].events = POLLIN | POLLERR | POLLHUP;
                    if (poll(fds, 1, 0) >= 0)
                    {
                        if (fds[0].revents & POLLIN)
                        {
                            size_t X11Operation;
                            if (error_t error = readAll(mX11RequestPipe[STDIN_FILENO], &X11Operation, sizeof(X11Operation)))
                            {
                                log(LOG_CRIT, "Cannot read from X11 request pipe: %s", strerror(error));
                                close(mX11ResponsePipe[STDIN_FILENO]);
                                mX11EventLoopActive = false;
                                break;
                            }
//                            log(LOG_DEBUG, "X11Operation: %d", X11Operation);

                            switch (X11Operation)
                            {
                            case X11_OP_StringToKeycode:
                            {
                                bool x11Error = false;
                                KeyCode keyCode = 0;
                                size_t length;
                                if (error_t error = readAll(mX11RequestPipe[STDIN_FILENO], &length, sizeof(length)))
                                {
                                    log(LOG_CRIT, "Cannot read from X11 request pipe: %s", strerror(error));
                                    close(mX11ResponsePipe[STDIN_FILENO]);
                                    mX11EventLoopActive = false;
                                    break;
                                }
                                if (length)
                                {
                                    char *str = new char[length + 1];
                                    str[length] = '\0';
                                    if (error_t error = readAll(mX11RequestPipe[STDIN_FILENO], str, length))
                                    {
                                        log(LOG_CRIT, "Cannot read from X11 request pipe: %s", strerror(error));
                                        close(mX11ResponsePipe[STDIN_FILENO]);
                                        mX11EventLoopActive = false;
                                        break;
                                    }
                                    KeySym keySym = XStringToKeysym(str);
                                    delete[] str;
                                    lockX11Error();
                                    keyCode = XKeysymToKeycode(mDisplay, keySym);
                                    x11Error = checkX11Error();
                                }

                                signal = x11Error ? 1 : 0;
                                if (error_t error = writeAll(mX11ResponsePipe[STDOUT_FILENO], &signal, sizeof(signal)))
                                {
                                    log(LOG_CRIT, "Cannot write to X11 response pipe: %s", strerror(error));
                                    close(mX11RequestPipe[STDIN_FILENO]);
                                    mX11EventLoopActive = false;
                                    break;
                                }

                                if (!x11Error)
                                    if (error_t error = writeAll(mX11ResponsePipe[STDOUT_FILENO], &keyCode, sizeof(keyCode)))
                                    {
                                        log(LOG_CRIT, "Cannot write to X11 response pipe: %s", strerror(error));
                                        close(mX11RequestPipe[STDIN_FILENO]);
                                        mX11EventLoopActive = false;
                                        break;
                                    }
                            }
                                break;

                            case X11_OP_KeycodeToString:
                            {
                                KeyCode keyCode;
                                bool x11Error = false;
                                if (error_t error = readAll(mX11RequestPipe[STDIN_FILENO], &keyCode, sizeof(keyCode)))
                                {
                                    log(LOG_CRIT, "Cannot read from X11 request pipe: %s", strerror(error));
                                    close(mX11ResponsePipe[STDIN_FILENO]);
                                    mX11EventLoopActive = false;
                                    break;
                                }
                                int keysymsPerKeycode;
                                lockX11Error();
                                KeySym *keySyms = XGetKeyboardMapping(mDisplay, keyCode, 1, &keysymsPerKeycode);
                                x11Error = checkX11Error();
                                char *str = NULL;

                                if (!x11Error)
                                {
                                    KeySym keySym = 0;
                                    if ((keysymsPerKeycode >= 2) && keySyms[1] && (keySyms[0] >= XK_a) && (keySyms[0] <= XK_z))
                                        keySym = keySyms[1];
                                    else if (keysymsPerKeycode >= 1)
                                        keySym = keySyms[0];

                                    if (keySym)
                                        str = XKeysymToString(keySym);
                                }

                                signal = x11Error ? 1 : 0;
                                if (error_t error = writeAll(mX11ResponsePipe[STDOUT_FILENO], &signal, sizeof(signal)))
                                {
                                    log(LOG_CRIT, "Cannot write to X11 response pipe: %s", strerror(error));
                                    close(mX11RequestPipe[STDIN_FILENO]);
                                    mX11EventLoopActive = false;
                                    break;
                                }

                                if (!x11Error)
                                {
                                    size_t length = 0;
                                    if (str)
                                        length = strlen(str);
                                    if (error_t error = writeAll(mX11ResponsePipe[STDOUT_FILENO], &length, sizeof(length)))
                                    {
                                        log(LOG_CRIT, "Cannot write to X11 response pipe: %s", strerror(error));
                                        close(mX11RequestPipe[STDIN_FILENO]);
                                        mX11EventLoopActive = false;
                                        break;
                                    }
                                    if (length)
                                    {
                                        if (error_t error = writeAll(mX11ResponsePipe[STDOUT_FILENO], str, length))
                                        {
                                            log(LOG_CRIT, "Cannot write to X11 response pipe: %s", strerror(error));
                                            close(mX11RequestPipe[STDIN_FILENO]);
                                            mX11EventLoopActive = false;
                                            break;
                                        }
                                    }
                                }
                            }
                                break;

                            case X11_OP_XGrabKey:
                            {
                                X11Shortcut X11shortcut;
                                bool x11Error = false;
                                if (error_t error = readAll(mX11RequestPipe[STDIN_FILENO], &X11shortcut.first, sizeof(X11shortcut.first)))
                                {
                                    log(LOG_CRIT, "Cannot read from X11 request pipe: %s", strerror(error));
                                    close(mX11ResponsePipe[STDIN_FILENO]);
                                    mX11EventLoopActive = false;
                                    break;
                                }
                                if (error_t error = readAll(mX11RequestPipe[STDIN_FILENO], &X11shortcut.second, sizeof(X11shortcut.second)))
                                {
                                    log(LOG_CRIT, "Cannot read from X11 request pipe: %s", strerror(error));
                                    close(mX11ResponsePipe[STDIN_FILENO]);
                                    mX11EventLoopActive = false;
                                    break;
                                }

                                QSet<unsigned int>::const_iterator lastAllModifiers = allModifiers.end();
                                for (QSet<unsigned int>::const_iterator modifiers = allModifiers.begin(); modifiers != lastAllModifiers; ++modifiers)
                                {
                                    lockX11Error();
                                    XGrabKey(mDisplay, X11shortcut.first, X11shortcut.second | *modifiers, rootWindow, False, GrabModeAsync, GrabModeAsync);
                                    bool x11e = checkX11Error();
                                    if (x11e)
                                        log(LOG_DEBUG, "XGrabKey: %02x + %02x", X11shortcut.first, X11shortcut.second | *modifiers);
                                    x11Error |= x11e;
                                }

                                signal = x11Error ? 1 : 0;
                                if (error_t error = writeAll(mX11ResponsePipe[STDOUT_FILENO], &signal, sizeof(signal)))
                                {
                                    log(LOG_CRIT, "Cannot write to X11 response pipe: %s", strerror(error));
                                    close(mX11RequestPipe[STDIN_FILENO]);
                                    mX11EventLoopActive = false;
                                    break;
                                }
                            }
                                break;

                            case X11_OP_XUngrabKey:
                            {
                                X11Shortcut X11shortcut;
                                bool x11Error = false;
                                if (error_t error = readAll(mX11RequestPipe[STDIN_FILENO], &X11shortcut.first, sizeof(X11shortcut.first)))
                                {
                                    log(LOG_CRIT, "Cannot read from X11 request pipe: %s", strerror(error));
                                    close(mX11ResponsePipe[STDIN_FILENO]);
                                    mX11EventLoopActive = false;
                                    break;
                                }
                                if (error_t error = readAll(mX11RequestPipe[STDIN_FILENO], &X11shortcut.second, sizeof(X11shortcut.second)))
                                {
                                    log(LOG_CRIT, "Cannot read from X11 request pipe: %s", strerror(error));
                                    close(mX11ResponsePipe[STDIN_FILENO]);
                                    mX11EventLoopActive = false;
                                    break;
                                }

                                lockX11Error();
                                QSet<unsigned int>::const_iterator lastAllModifiers = allModifiers.end();
                                for (QSet<unsigned int>::const_iterator modifiers = allModifiers.begin(); modifiers != lastAllModifiers; ++modifiers)
                                    XUngrabKey(mDisplay, X11shortcut.first, X11shortcut.second | *modifiers, rootWindow);
                                x11Error = checkX11Error();

                                signal = x11Error ? 1 : 0;
                                if (error_t error = writeAll(mX11ResponsePipe[STDOUT_FILENO], &signal, sizeof(signal)))
                                {
                                    log(LOG_CRIT, "Cannot write to X11 response pipe: %s", strerror(error));
                                    close(mX11RequestPipe[STDIN_FILENO]);
                                    mX11EventLoopActive = false;
                                    break;
                                }
                            }
                                break;

                            case X11_OP_XGrabKeyboard:
                            {
                                lockX11Error();
                                int result = XGrabKeyboard(mDisplay, rootWindow, False, GrabModeAsync, GrabModeAsync, CurrentTime);
                                bool x11Error = checkX11Error();
                                if (!result && x11Error)
                                    result = -1;

                                if (error_t error = writeAll(mX11ResponsePipe[STDOUT_FILENO], &result, sizeof(result)))
                                {
                                    log(LOG_CRIT, "Cannot write to X11 response pipe: %s", strerror(error));
                                    close(mX11RequestPipe[STDIN_FILENO]);
                                    mX11EventLoopActive = false;
                                    break;
                                }
                                mDataMutex.lock();
                                mGrabbingShortcut = true;
                                mDataMutex.unlock();
                            }
                                break;

                            case X11_OP_XUngrabKeyboard:
                            {
                                lockX11Error();
                                XUngrabKeyboard(mDisplay, CurrentTime);
                                bool x11Error = checkX11Error();

                                signal = x11Error ? 1 : 0;
                                if (error_t error = writeAll(mX11ResponsePipe[STDOUT_FILENO], &signal, sizeof(signal)))
                                {
                                    log(LOG_CRIT, "Cannot write to X11 response pipe: %s", strerror(error));
                                    close(mX11RequestPipe[STDIN_FILENO]);
                                    mX11EventLoopActive = false;
                                    break;
                                }

                                mDataMutex.lock();
                                mGrabbingShortcut = false;
                                mDataMutex.unlock();
                            }
                                break;

                            }
                        }
                    }
                }
            }
        }
    }

    lockX11Error();
    XUngrabKey(mDisplay, AnyKey, AnyModifier, rootWindow);
    XSetErrorHandler(oldx11ErrorHandler);
    XCloseDisplay(mDisplay);
    checkX11Error(0);
}

void Core::serviceAppeared(const QString &service, const QString &id)
{
    log(LOG_DEBUG, "serviceAppeared '%s' '%s'", qPrintable(service), qPrintable(id));

    QMutexLocker lock(&mDataMutex);

    if (service != id)
    {
        mServiceNamesByServiceId[id].insert(service);
        if (mPreferredServiceNameByServiceId.find(id) == mPreferredServiceNameByServiceId.end())
            mPreferredServiceNameByServiceId[id] = service;
        mServiceIdByServiceName[service] = id;

        DBusPathsByDBusService::iterator dBusPathsByDBusService = mDBusPathsByDBusService.find(id);
        if (dBusPathsByDBusService != mDBusPathsByDBusService.end())
        {
            DBusPaths::iterator lastDBusPaths = dBusPathsByDBusService.value().end();
            for (DBusPaths::iterator dBusPaths = dBusPathsByDBusService.value().begin(); dBusPaths != lastDBusPaths; ++dBusPaths)
            {
                const QDBusObjectPath &path = *dBusPaths;

                IdByDBusClient::iterator idByDBusClient = mIdByDBusClient.find(qMakePair(id, path));
                if (idByDBusClient != mIdByDBusClient.end())
                {
                    log(LOG_INFO, "Activating DBus action for '%s' @ %s", qPrintable(service), qPrintable(path.path()));

                    ShortcutAndActionById::iterator shortcutAndActionById = mShortcutAndActionById.find(idByDBusClient.value());
                    if (shortcutAndActionById != mShortcutAndActionById.end())
                        dynamic_cast<DBusAction*>(shortcutAndActionById.value().second)->appeared(QDBusConnection::sessionBus());
                }
            }
        }
    }
}

void Core::serviceDisappeared(const QString &service, const QString &id)
{
    log(LOG_DEBUG, "serviceDisappeared '%s' '%s'", qPrintable(service), qPrintable(id));

    QMutexLocker lock(&mDataMutex);

    DBusPathsByDBusService::iterator dBusPathsByDBusService = mDBusPathsByDBusService.find(id);
    if (dBusPathsByDBusService != mDBusPathsByDBusService.end())
    {
        size_t pathsCount = dBusPathsByDBusService.value().size();
        DBusPaths::iterator lastDBusPaths = dBusPathsByDBusService.value().end();
        for (DBusPaths::iterator dBusPaths = dBusPathsByDBusService.value().begin(); dBusPaths != lastDBusPaths; ++dBusPaths)
        {
            const QDBusObjectPath &path = *dBusPaths;

            IdByDBusClient::iterator idByDBusClient = mIdByDBusClient.find(qMakePair(id, path));
            if (idByDBusClient != mIdByDBusClient.end())
            {
                log(LOG_INFO, "Disactivating DBus action for '%s' @ %s", qPrintable(service), qPrintable(path.path()));

                ShortcutAndActionById::iterator shortcutAndActionById = mShortcutAndActionById.find(idByDBusClient.value());
                if (shortcutAndActionById != mShortcutAndActionById.end())
                {
                    const QString &shortcut = shortcutAndActionById.value().first;

                    X11Shortcut X11shortcut = mX11ByShortcut[shortcut];

                    IdsByShortcut::iterator idsByShortcut = mIdsByShortcut.find(shortcut);
                    if (idsByShortcut != mIdsByShortcut.end())
                    {
                        idsByShortcut.value().remove(idByDBusClient.value());
                        if (idsByShortcut.value().isEmpty())
                        {
                            mIdsByShortcut.erase(idsByShortcut);

                            if (!remoteXUngrabKey(X11shortcut))
                                log(LOG_WARNING, "Cannot ungrab shortcut '%s'", qPrintable(shortcut));
                        }
                    }

                    if (id != service)
                        dynamic_cast<DBusAction*>(shortcutAndActionById.value().second)->disappeared();
                    else
                    {
                        delete shortcutAndActionById.value().second;
                        mShortcutAndActionById.erase(shortcutAndActionById);
                        mIdByDBusClient.erase(idByDBusClient);
                        --pathsCount;
                    }
                }
            }
        }
        if (!pathsCount)
            mDBusPathsByDBusService.erase(dBusPathsByDBusService);
    }


    if (id == service)
    {
        mPreferredServiceNameByServiceId.remove(id);
        mServiceNamesByServiceId.remove(id);
    }
    else
    {
        ServiceNamesByServiceId::iterator serviceNamesByServiceId = mServiceNamesByServiceId.find(id);
        if (serviceNamesByServiceId != mServiceNamesByServiceId.end())
        {
            serviceNamesByServiceId.value().remove(service);
            if (serviceNamesByServiceId.value().isEmpty())
                mServiceNamesByServiceId.erase(serviceNamesByServiceId);
        }

        PreferredServiceNameByServiceId::iterator preferredServiceNameByServiceId = mPreferredServiceNameByServiceId.find(id);
        if ((preferredServiceNameByServiceId != mPreferredServiceNameByServiceId.end()) && (preferredServiceNameByServiceId.value() == service))
            mPreferredServiceNameByServiceId.erase(preferredServiceNameByServiceId);

        mServiceIdByServiceName.remove(service);
    }
}

KeyCode Core::remoteStringToKeycode(const QString &str)
{
    size_t X11Operation = X11_OP_StringToKeycode;
    size_t length = str.length();
    if (error_t error = writeAll(mX11RequestPipe[STDOUT_FILENO], &X11Operation, sizeof(X11Operation)))
    {
        log(LOG_CRIT, "Cannot write to X11 request pipe: %s", strerror(error));
        qApp->quit();
        return 0;
    }
    if (error_t error = writeAll(mX11RequestPipe[STDOUT_FILENO], &length, sizeof(length)))
    {
        log(LOG_CRIT, "Cannot write to X11 request pipe: %s", strerror(error));
        qApp->quit();
        return 0;
    }
    if (length)
    {
        if (error_t error = writeAll(mX11RequestPipe[STDOUT_FILENO], qPrintable(str), length))
        {
            log(LOG_CRIT, "Cannot write to X11 request pipe: %s", strerror(error));
            qApp->quit();
            return 0;
        }
    }
    wakeX11Thread();

    char signal;
    if (error_t error = readAll(mX11ResponsePipe[STDIN_FILENO], &signal, sizeof(signal)))
    {
        log(LOG_CRIT, "Cannot read from X11 response pipe: %s", strerror(error));
        qApp->quit();
        return 0;
    }
    if (signal)
        return 0;

    KeyCode keyCode;
    if (error_t error = readAll(mX11ResponsePipe[STDIN_FILENO], &keyCode, sizeof(keyCode)))
    {
        log(LOG_CRIT, "Cannot read from X11 response pipe: %s", strerror(error));
        qApp->quit();
        return 0;
    }
    return keyCode;
}

QString Core::remoteKeycodeToString(KeyCode keyCode)
{
    QString result;

    size_t X11Operation = X11_OP_KeycodeToString;
    if (error_t error = writeAll(mX11RequestPipe[STDOUT_FILENO], &X11Operation, sizeof(X11Operation)))
    {
        log(LOG_CRIT, "Cannot write to X11 request pipe: %s", strerror(error));
        qApp->quit();
        return QString();
    }
    if (error_t error = writeAll(mX11RequestPipe[STDOUT_FILENO], &keyCode, sizeof(keyCode)))
    {
        log(LOG_CRIT, "Cannot write to X11 request pipe: %s", strerror(error));
        qApp->quit();
        return QString();
    }
    wakeX11Thread();

    char signal;
    if (error_t error = readAll(mX11ResponsePipe[STDIN_FILENO], &signal, sizeof(signal)))
    {
        log(LOG_CRIT, "Cannot read from X11 response pipe: %s", strerror(error));
        qApp->quit();
        return QString();
    }
    if (signal)
        return QString();

    size_t length;
    if (error_t error = readAll(mX11ResponsePipe[STDIN_FILENO], &length, sizeof(length)))
    {
        log(LOG_CRIT, "Cannot read from X11 response pipe: %s", strerror(error));
        qApp->quit();
        return QString();
    }
    if (length)
    {
        char *str = new char[length + 1];
        str[length] = '\0';
        if (error_t error = readAll(mX11ResponsePipe[STDIN_FILENO], str, length))
        {
            log(LOG_CRIT, "Cannot read from X11 response pipe: %s", strerror(error));
            qApp->quit();
            return QString();
        }
        result = str;

        delete[] str;
    }

    return result;
}

bool Core::remoteXGrabKey(const X11Shortcut &X11shortcut)
{
    size_t X11Operation = X11_OP_XGrabKey;
    if (error_t error = writeAll(mX11RequestPipe[STDOUT_FILENO], &X11Operation, sizeof(X11Operation)))
    {
        log(LOG_CRIT, "Cannot write to X11 request pipe: %s", strerror(error));
        qApp->quit();
        return false;
    }
    if (error_t error = writeAll(mX11RequestPipe[STDOUT_FILENO], &X11shortcut.first, sizeof(X11shortcut.first)))
    {
        log(LOG_CRIT, "Cannot write to X11 request pipe: %s", strerror(error));
        qApp->quit();
        return false;
    }
    if (error_t error = writeAll(mX11RequestPipe[STDOUT_FILENO], &X11shortcut.second, sizeof(X11shortcut.second)))
    {
        log(LOG_CRIT, "Cannot write to X11 request pipe: %s", strerror(error));
        qApp->quit();
        return false;
    }
    wakeX11Thread();

    char signal;
    if (error_t error = readAll(mX11ResponsePipe[STDIN_FILENO], &signal, sizeof(signal)))
    {
        log(LOG_CRIT, "Cannot read from X11 response pipe: %s", strerror(error));
        qApp->quit();
        return false;
    }
    if (signal)
        return false;

    return true;
}

bool Core::remoteXUngrabKey(const X11Shortcut &X11shortcut)
{
    size_t X11Operation = X11_OP_XUngrabKey;
    if (error_t error = writeAll(mX11RequestPipe[STDOUT_FILENO], &X11Operation, sizeof(X11Operation)))
    {
        log(LOG_CRIT, "Cannot write to X11 request pipe: %s", strerror(error));
        qApp->quit();
        return false;
    }
    if (error_t error = writeAll(mX11RequestPipe[STDOUT_FILENO], &X11shortcut.first, sizeof(X11shortcut.first)))
    {
        log(LOG_CRIT, "Cannot write to X11 request pipe: %s", strerror(error));
        qApp->quit();
        return false;
    }
    if (error_t error = writeAll(mX11RequestPipe[STDOUT_FILENO], &X11shortcut.second, sizeof(X11shortcut.second)))
    {
        log(LOG_CRIT, "Cannot write to X11 request pipe: %s", strerror(error));
        qApp->quit();
        return false;
    }
    wakeX11Thread();

    char signal;
    if (error_t error = readAll(mX11ResponsePipe[STDIN_FILENO], &signal, sizeof(signal)))
    {
        log(LOG_CRIT, "Cannot read from X11 response pipe: %s", strerror(error));
        qApp->quit();
        return false;
    }
    if (signal)
        return false;

    return true;
}

QString Core::grabOrReuseKey(const X11Shortcut &X11shortcut, const QString &shortcut)
{
    IdsByShortcut::iterator idsByShortcut = mIdsByShortcut.find(shortcut);
    if ((idsByShortcut != mIdsByShortcut.end()) && (!idsByShortcut.value().isEmpty()))
        return shortcut;

    if (!remoteXGrabKey(X11shortcut))
    {
        log(LOG_WARNING, "Cannot grab shortcut '%s'", qPrintable(shortcut));
        return QString();
    }

    return shortcut;
}


Core::X11Shortcut Core::ShortcutToX11(const QString &shortcut)
{
    X11Shortcut result(0, 0);

    QStringList parts = shortcut.split('+');

    size_t m = parts.size();
    for (size_t i = 0; i < m - 1; ++i)
    {
        if (parts[i] == "Shift")
            result.second |= ShiftMask;
        else if (parts[i] == "Control")
            result.second |= ControlMask;
        else if (parts[i] == "Alt")
            result.second |= AltMask;
        else if (parts[i] == "Meta")
            result.second |= MetaMask;
        else if (parts[i] == "Level3")
            result.second |= Level3Mask;
        else if (parts[i] == "Level5")
            result.second |= Level5Mask;
        else
            throw false;
    }
    if (m)
    {
        KeyCode keyCode = remoteStringToKeycode(parts[m - 1]);
        if (!keyCode)
            throw false;

        result.first = keyCode;
    }

    return result;
}

QString Core::X11ToShortcut(const X11Shortcut &X11shortcut)
{
    QString result;

    if (X11shortcut.second & ShiftMask)
        result += "Shift+";
    if (X11shortcut.second & ControlMask)
        result += "Control+";
    if (X11shortcut.second & AltMask)
        result += "Alt+";
    if (X11shortcut.second & MetaMask)
        result += "Meta+";
    if (X11shortcut.second & Level3Mask)
        result += "Level3+";
    if (X11shortcut.second & Level5Mask)
        result += "Level5+";

    QString key = remoteKeycodeToString(X11shortcut.first);
    if (key.isEmpty())
        throw false;

    result += key;

    return result;
}

bool Core::addActionCommon(const QString &shortcut, X11Shortcut &X11shortcut, QString &usedShortcut)
{
    try
    {
        X11shortcut = ShortcutToX11(shortcut);
    }
    catch (bool)
    {
        log(LOG_WARNING, "Cannot extract keycode and modifiers from shortcut '%s'", qPrintable(shortcut));
        return false;
    }

    try
    {
        ShortcutByX11::const_iterator shortcutByX11 = mShortcutByX11.find(X11shortcut);
        if (shortcutByX11 != mShortcutByX11.end())
        {
            usedShortcut = shortcutByX11.value();
        }
        else
        {
            usedShortcut = X11ToShortcut(X11shortcut);
            mShortcutByX11[X11shortcut] = usedShortcut;
        }
    }
    catch (bool)
    {
        log(LOG_WARNING, "Cannot get back shortcut '%s'", qPrintable(shortcut));
        return false;
    }

    if (shortcut != usedShortcut)
        log(LOG_INFO, "Using shortcut '%s' instead of '%s'", qPrintable(usedShortcut), qPrintable(shortcut));

    X11ByShortcut::const_iterator x11ByShortcut = mX11ByShortcut.find(usedShortcut);
    if (x11ByShortcut == mX11ByShortcut.end())
        mX11ByShortcut[usedShortcut] = X11shortcut;

    return true;
}

QPair<QString, qulonglong> Core::addOrRegisterDBusAction(const QString &shortcut, const QString &service, const QDBusObjectPath &path, const QString &description, const QString &sender)
{
    DBusClient dBusClient = qMakePair(service, path);

//    QMutexLocker lock(&mDataMutex);

    DBusPathsByDBusService::iterator dBusPathsByDBusService = mDBusPathsByDBusService.find(service);
    if (dBusPathsByDBusService != mDBusPathsByDBusService.end())
    {
        DBusPaths::iterator dBusPaths = dBusPathsByDBusService.value().find(path);
        if (dBusPaths != dBusPathsByDBusService.value().end())
        {
            log(LOG_WARNING, "DBus client already registered for '%s' @ %s", qPrintable(service), qPrintable(path.path()));
            IdByDBusClient::const_iterator idByDBusClient = mIdByDBusClient.find(dBusClient);
            if (idByDBusClient != mIdByDBusClient.end())
                return qMakePair(mShortcutAndActionById[idByDBusClient.value()].first, idByDBusClient.value());
            else
                return qMakePair(QString(), 0ull);
        }
        else
        {
            dBusPathsByDBusService.value().insert(path);
        }
    }
    else
        mDBusPathsByDBusService[service].insert(path);

    X11Shortcut X11shortcut;
    QString usedShortcut;

    if (!addActionCommon(shortcut, X11shortcut, usedShortcut))
    {
        dBusPathsByDBusService = mDBusPathsByDBusService.find(service);
        dBusPathsByDBusService.value().remove(path);
        if (dBusPathsByDBusService.value().isEmpty())
            mDBusPathsByDBusService.erase(dBusPathsByDBusService);
        return qMakePair(QString(), 0ull);
    }

    QString newShortcut = grabOrReuseKey(X11shortcut, usedShortcut);
    if (newShortcut.isEmpty())
        return qMakePair(QString(), 0ull);


    qulonglong id = ++mLastId;

    mIdsByShortcut[newShortcut].insert(id);
    mIdByDBusClient[dBusClient] = id;
    mShortcutAndActionById[id] = qMakePair<QString, BaseAction*>(newShortcut, sender.isEmpty() ? new DBusAction(service, path, description) : new DBusAction(QDBusConnection::sessionBus(), service, path, description, service != sender));

    log(LOG_INFO, "addDBusAction shortcut:'%s' id:%llu", qPrintable(newShortcut), id);

    return qMakePair(newShortcut, id);
}

void Core::addDBusAction(QPair<QString, qulonglong> &result, const QString &shortcut, const QDBusObjectPath &path, const QString &description, const QString &sender)
{
    log(LOG_INFO, "addDBusAction shortcut:'%s' path:'%s' description:'%s' sender:'%s'", qPrintable(shortcut), qPrintable(path.path()), qPrintable(description), qPrintable(sender));

    QMutexLocker lock(&mDataMutex);

    QString service = sender;
    PreferredServiceNameByServiceId::const_iterator preferredServiceNameByServiceId = mPreferredServiceNameByServiceId.find(sender);
    if (preferredServiceNameByServiceId != mPreferredServiceNameByServiceId.end())
        service = preferredServiceNameByServiceId.value();

    result = addOrRegisterDBusAction(shortcut, service, path, description, sender);

    saveConfig();
}

qulonglong Core::registerDBusAction(const QString &shortcut, const QString &service, const QDBusObjectPath &path, const QString &description)
{
    log(LOG_INFO, "registerDBusAction shortcut:'%s' service:'%s' path:'%s' description:'%s'", qPrintable(shortcut), qPrintable(service), qPrintable(path.path()), qPrintable(description));

    QMutexLocker lock(&mDataMutex);

    return addOrRegisterDBusAction(shortcut, service, path, description, QString()).second;
}

void Core::addMethodAction(QPair<QString, qulonglong> &result, const QString &shortcut, const QString &service, const QDBusObjectPath &path, const QString &interface, const QString &method, const QString &description)
{
    log(LOG_INFO, "addMethodAction shortcut:'%s' service:'%s' path:'%s' interface:'%s' method:'%s' description:'%s'", qPrintable(shortcut), qPrintable(service), qPrintable(path.path()), qPrintable(interface), qPrintable(method), qPrintable(description));

    QMutexLocker lock(&mDataMutex);

    X11Shortcut X11shortcut;
    QString usedShortcut;

    if (!addActionCommon(shortcut, X11shortcut, usedShortcut))
    {
        result = qMakePair(QString(), 0ull);
        return;
    }

    QString newShortcut = grabOrReuseKey(X11shortcut, usedShortcut);
    if (newShortcut.isEmpty())
    {
        result = qMakePair(QString(), 0ull);
        return;
    }


    qulonglong id = ++mLastId;

    mIdsByShortcut[newShortcut].insert(id);
    mShortcutAndActionById[id] = qMakePair<QString, BaseAction*>(newShortcut, new MethodAction(QDBusConnection::sessionBus(), service, path, interface, method, description));

    log(LOG_INFO, "addMethodAction shortcut:'%s' id:%llu", qPrintable(newShortcut), id);

    saveConfig();

    result = qMakePair(newShortcut, id);
}

qulonglong Core::registerMethodAction(const QString &shortcut, const QString &service, const QDBusObjectPath &path, const QString &interface, const QString &method, const QString &description)
{
    QPair<QString, qulonglong> result;
    addMethodAction(result, shortcut, service, path, interface, method, description);
    return result.second;
}

void Core::addCommandAction(QPair<QString, qulonglong> &result, const QString &shortcut, const QString &command, const QStringList &arguments, const QString &description)
{
    log(LOG_INFO, "addCommandAction shortcut:'%s' command:'%s' arguments:'%s' description:'%s'", qPrintable(shortcut), qPrintable(command), qPrintable(joinToString(arguments, "", "' '", "")), qPrintable(description));

    QMutexLocker lock(&mDataMutex);

    X11Shortcut X11shortcut;
    QString usedShortcut;

    if (!addActionCommon(shortcut, X11shortcut, usedShortcut))
    {
        result = qMakePair(QString(), 0ull);
        return;
    }

    QString newShortcut = grabOrReuseKey(X11shortcut, usedShortcut);
    if (newShortcut.isEmpty())
    {
        result = qMakePair(QString(), 0ull);
        return;
    }


    qulonglong id = ++mLastId;

    mIdsByShortcut[newShortcut].insert(id);
    mShortcutAndActionById[id] = qMakePair<QString, BaseAction*>(newShortcut, new CommandAction(this, command, arguments, description));

    log(LOG_INFO, "addCommandAction shortcut:'%s' id:%llu", qPrintable(newShortcut), id);

    saveConfig();

    result = qMakePair(newShortcut, id);
}

qulonglong Core::registerCommandAction(const QString &shortcut, const QString &command, const QStringList &arguments, const QString &description)
{
    QPair<QString, qulonglong> result;
    addCommandAction(result, shortcut, command, arguments, description);
    return result.second;
}

void Core::modifyDBusAction(qulonglong& result, const QDBusObjectPath &path, const QString &description, const QString &sender)
{
    log(LOG_INFO, "modifyDBusAction path:'%s' description:'%s' sender:'%s'", qPrintable(path.path()), qPrintable(description), qPrintable(sender));

    DBusClient dBusClient = qMakePair(sender, path);

    QMutexLocker lock(&mDataMutex);

    IdByDBusClient::iterator idByDBusClient = mIdByDBusClient.find(dBusClient);
    if (idByDBusClient == mIdByDBusClient.end())
    {
        log(LOG_WARNING, "No action registered for '%s' @ %s", qPrintable(sender), qPrintable(path.path()));
        result = 0ull;
        return;
    }

    qulonglong id = idByDBusClient.value();

    mShortcutAndActionById[id].second->setDescription(description);

    saveConfig();

    result = id;
}

void Core::modifyActionDescription(bool &result, const qulonglong &id, const QString &description)
{
    log(LOG_INFO, "modifyActionDescription id:%llu description:'%s'", id, qPrintable(description));

    QMutexLocker lock(&mDataMutex);

    ShortcutAndActionById::iterator shortcutAndActionById = mShortcutAndActionById.find(id);
    if (shortcutAndActionById == mShortcutAndActionById.end())
    {
        log(LOG_WARNING, "No action registered with id #%llu", id);
        result = false;
        return;
    }

    BaseAction *action = shortcutAndActionById.value().second;

    if ((strcmp(action->type(), MethodAction::id())) && (strcmp(action->type(), CommandAction::id())))
    {
        log(LOG_WARNING, "modifyActionDescription attempts to modify action of type '%s'", action->type());
        result = false;
        return;
    }

    action->setDescription(description);

    saveConfig();

    result = true;
}

void Core::modifyMethodAction(bool &result, const qulonglong &id, const QString &service, const QDBusObjectPath &path, const QString &interface, const QString &method, const QString &description)
{
    log(LOG_INFO, "modifyMethodAction id:%llu service:'%s' path:'%s' interface:'%s' method:'%s' description:'%s'", id, qPrintable(service), qPrintable(path.path()), qPrintable(interface), qPrintable(method), qPrintable(description));

    QMutexLocker lock(&mDataMutex);

    ShortcutAndActionById::iterator shortcutAndActionById = mShortcutAndActionById.find(id);
    if (shortcutAndActionById == mShortcutAndActionById.end())
    {
        log(LOG_WARNING, "No action registered with id #%llu", id);
        result = false;
        return;
    }

    BaseAction *action = shortcutAndActionById.value().second;

    if (strcmp(action->type(), MethodAction::id()))
    {
        log(LOG_WARNING, "modifyMethodAction attempts to modify action of type '%s'", action->type());
        result = false;
        return;
    }

    delete action;
    shortcutAndActionById.value().second = new MethodAction(QDBusConnection::sessionBus(), service, path, interface, method, description);

    saveConfig();

    result = true;
}

void Core::modifyCommandAction(bool &result, const qulonglong &id, const QString &command, const QStringList &arguments, const QString &description)
{
    log(LOG_INFO, "modifyCommandAction id:%llu command:'%s' arguments:'%s' description:'%s'", id, qPrintable(command), qPrintable(joinToString(arguments, "", "' '", "")), qPrintable(description));

    QMutexLocker lock(&mDataMutex);

    ShortcutAndActionById::iterator shortcutAndActionById = mShortcutAndActionById.find(id);
    if (shortcutAndActionById == mShortcutAndActionById.end())
    {
        log(LOG_WARNING, "No action registered with id #%llu", id);
        result = false;
        return;
    }

    BaseAction *action = shortcutAndActionById.value().second;

    if (strcmp(action->type(), CommandAction::id()))
    {
        log(LOG_WARNING, "modifyMethodAction attempts to modify action of type '%s'", action->type());
        result = false;
        return;
    }

    delete action;
    shortcutAndActionById.value().second = new CommandAction(this, command, arguments, description);

    saveConfig();

    result = true;
}

void Core::enableDBusAction(bool &result, const QDBusObjectPath &path, bool enabled, const QString &sender)
{
    log(LOG_INFO, "enableDBusAction path:'%s' enabled:%s sender:'%s'", qPrintable(path.path()), enabled ? " true" : "false", qPrintable(sender));

    DBusClient dBusClient = qMakePair(sender, path);

    QMutexLocker lock(&mDataMutex);

    IdByDBusClient::iterator idByDBusClient = mIdByDBusClient.find(dBusClient);
    if (idByDBusClient == mIdByDBusClient.end())
    {
        log(LOG_WARNING, "No action registered for '%s' @ %s", qPrintable(sender), qPrintable(path.path()));
        result = false;
        return;
    }

    qulonglong id = idByDBusClient.value();

    mShortcutAndActionById[id].second->setEnabled(enabled);

    saveConfig();

    result = true;
}

void Core::isDBusActionEnabled(bool &enabled, const QDBusObjectPath &path, const QString &sender)
{
    log(LOG_INFO, "isDBusActionEnabled path:'%s' sender:'%s'", qPrintable(path.path()), qPrintable(sender));

    DBusClient dBusClient = qMakePair(sender, path);

    enabled = false;

    QMutexLocker lock(&mDataMutex);

    IdByDBusClient::iterator idByDBusClient = mIdByDBusClient.find(dBusClient);
    if (idByDBusClient == mIdByDBusClient.end())
    {
        log(LOG_WARNING, "No action registered for '%s' @ %s", qPrintable(sender), qPrintable(path.path()));
        return;
    }

    enabled = mShortcutAndActionById[idByDBusClient.value()].second->isEnabled();
}

void Core::enableAction(bool &result, qulonglong id, bool enabled)
{
    log(LOG_INFO, "enableAction id:%llu enabled:%s", id, enabled ? "true" : " false");

    QMutexLocker lock(&mDataMutex);

    ShortcutAndActionById::iterator shortcutAndActionById = mShortcutAndActionById.find(id);
    if (shortcutAndActionById == mShortcutAndActionById.end())
    {
        log(LOG_WARNING, "No action registered with id #%llu", id);
        result = false;
        return;
    }

    shortcutAndActionById.value().second->setEnabled(enabled);

    saveConfig();

    result = true;
}

void Core::isActionEnabled(bool &enabled, qulonglong id)
{
    log(LOG_INFO, "isActionEnabled id:%llu", id);

    enabled = false;

    QMutexLocker lock(&mDataMutex);

    ShortcutAndActionById::iterator shortcutAndActionById = mShortcutAndActionById.find(id);
    if (shortcutAndActionById == mShortcutAndActionById.end())
    {
        log(LOG_WARNING, "No action registered with id #%llu", id);
        return;
    }

    enabled = shortcutAndActionById.value().second->isEnabled();
}

void Core::changeDBusShortcut(QPair<QString, qulonglong> &result, const QDBusObjectPath &path, const QString &shortcut, const QString &sender)
{
    log(LOG_INFO, "changeDBusShortcut path:'%s' shortcut:'%s' sender:'%s'", qPrintable(path.path()), qPrintable(shortcut), qPrintable(sender));

    if (shortcut.isEmpty())
    {
        result = qMakePair(QString(), 0ull);
        return;
    }

    DBusClient dBusClient = qMakePair(sender, path);

    QMutexLocker lock(&mDataMutex);

    IdByDBusClient::iterator idByDBusClient = mIdByDBusClient.find(dBusClient);
    if (idByDBusClient == mIdByDBusClient.end())
    {
        log(LOG_WARNING, "No action registered for '%s' @ %s", qPrintable(sender), qPrintable(path.path()));
        result = qMakePair(QString(), 0ull);
        return;
    }

    qulonglong id = idByDBusClient.value();

    X11Shortcut X11shortcut;
    QString usedShortcut;

    if (!addActionCommon(shortcut, X11shortcut, usedShortcut))
    {
        result = qMakePair(QString(), 0ull);
        return;
    }

    ShortcutAndActionById::iterator shortcutAndActionById = mShortcutAndActionById.find(id);

    QString oldShortcut = shortcutAndActionById.value().first;
    QString newShortcut = grabOrReuseKey(X11shortcut, usedShortcut);
    if (newShortcut.isEmpty())
    {
        result = qMakePair(QString(), 0ull);
        return;
    }

    if (oldShortcut != usedShortcut)
    {
        IdsByShortcut::iterator idsByShortcut = mIdsByShortcut.find(oldShortcut);
        if (idsByShortcut != mIdsByShortcut.end())
        {
            idsByShortcut.value().remove(id);
            if (idsByShortcut.value().isEmpty())
            {
                mIdsByShortcut.erase(idsByShortcut);

                if (!remoteXUngrabKey(mX11ByShortcut[oldShortcut]))
                    log(LOG_WARNING, "Cannot ungrab shortcut '%s'", qPrintable(shortcut));
            }
        }

        mIdsByShortcut[newShortcut].insert(id);
        shortcutAndActionById.value().first = newShortcut;
    }

    dynamic_cast<DBusAction*>(shortcutAndActionById.value().second)->shortcutChanged(oldShortcut, newShortcut);

    saveConfig();

    result = qMakePair(newShortcut, id);
}

void Core::changeShortcut(QString &result, const qulonglong &id, const QString &shortcut)
{
    log(LOG_INFO, "changeShortcut id:%llu shortcut:'%s'", id, qPrintable(shortcut));

    if (shortcut.isEmpty())
    {
        result = QString();
        return;
    }

    QMutexLocker lock(&mDataMutex);

    ShortcutAndActionById::iterator shortcutAndActionById = mShortcutAndActionById.find(id);
    if (shortcutAndActionById == mShortcutAndActionById.end())
    {
        log(LOG_WARNING, "No action registered with id #%llu", id);
        result = QString();
        return;
    }

    X11Shortcut X11shortcut;
    QString usedShortcut;

    if (!addActionCommon(shortcut, X11shortcut, usedShortcut))
    {
        result = QString();
        return;
    }

    QString oldShortcut = shortcutAndActionById.value().first;
    QString newShortcut = grabOrReuseKey(X11shortcut, usedShortcut);
    if (newShortcut.isEmpty())
    {
        result = QString();
        return;
    }

    if (oldShortcut != usedShortcut)
    {
        IdsByShortcut::iterator idsByShortcut = mIdsByShortcut.find(oldShortcut);
        if (idsByShortcut != mIdsByShortcut.end())
        {
            idsByShortcut.value().remove(id);
            if (idsByShortcut.value().isEmpty())
            {
                mIdsByShortcut.erase(idsByShortcut);

                if (!remoteXUngrabKey(mX11ByShortcut[oldShortcut]))
                    log(LOG_WARNING, "Cannot ungrab shortcut '%s'", qPrintable(shortcut));
            }
        }

        mIdsByShortcut[newShortcut].insert(id);
        shortcutAndActionById.value().first = newShortcut;

        if (!strcmp(shortcutAndActionById.value().second->type(), DBusAction::id()))
            dynamic_cast<DBusAction*>(shortcutAndActionById.value().second)->shortcutChanged(oldShortcut, newShortcut);
    }

    saveConfig();

    result = newShortcut;
}

void Core::swapActions(bool &result, const qulonglong &id1, const qulonglong &id2)
{
    log(LOG_INFO, "swapActions id1:%llu id2:%llu", id1, id2);

    QMutexLocker lock(&mDataMutex);

    ShortcutAndActionById::iterator shortcutAndActionById1 = mShortcutAndActionById.find(id1);
    if (shortcutAndActionById1 == mShortcutAndActionById.end())
    {
        log(LOG_WARNING, "No action registered with id #%llu", id1);
        result = false;
        return;
    }

    ShortcutAndActionById::iterator shortcutAndActionById2 = mShortcutAndActionById.find(id2);
    if (shortcutAndActionById2 == mShortcutAndActionById.end())
    {
        log(LOG_WARNING, "No action registered with id #%llu", id2);
        result = false;
        return;
    }

    if (shortcutAndActionById1.value().first != shortcutAndActionById2.value().first)
    {
        log(LOG_WARNING, "swapActions attempts to swap action assigned to different shortcuts");
        result = false;
        return;
    }

    std::swap(shortcutAndActionById1.value().second, shortcutAndActionById2.value().second);

    saveConfig();

    result = true;
}

void Core::removeDBusAction(qulonglong &result, const QDBusObjectPath &path, const QString &sender)
{
    log(LOG_INFO, "removeDBusAction path:'%s' sender:'%s'", qPrintable(path.path()), qPrintable(sender));

    DBusClient dBusClient = qMakePair(sender, path);

    QMutexLocker lock(&mDataMutex);

    IdByDBusClient::iterator idByDBusClient = mIdByDBusClient.find(dBusClient);
    if (idByDBusClient == mIdByDBusClient.end())
    {
        log(LOG_WARNING, "No action registered for '%s' @ %s", qPrintable(sender), qPrintable(path.path()));
        result = 0ull;
        return;
    }

    qulonglong id = idByDBusClient.value();

    ShortcutAndActionById::iterator shortcutAndActionById = mShortcutAndActionById.find(id);
    QString shortcut = shortcutAndActionById.value().first;

    X11Shortcut X11shortcut = mX11ByShortcut[shortcut];

    delete shortcutAndActionById.value().second;
    mShortcutAndActionById.erase(shortcutAndActionById);
    mIdByDBusClient.remove(dBusClient);

    IdsByShortcut::iterator idsByShortcut = mIdsByShortcut.find(shortcut);
    if (idsByShortcut != mIdsByShortcut.end())
    {
        idsByShortcut.value().remove(id);
        if (idsByShortcut.value().isEmpty())
        {
            mIdsByShortcut.erase(idsByShortcut);

            if (!remoteXUngrabKey(X11shortcut))
                log(LOG_WARNING, "Cannot ungrab shortcut '%s'", qPrintable(shortcut));
        }
    }

    DBusPathsByDBusService::iterator dBusPathsByDBusService = mDBusPathsByDBusService.find(sender);
    dBusPathsByDBusService.value().remove(path);
    if (dBusPathsByDBusService.value().isEmpty())
        mDBusPathsByDBusService.erase(dBusPathsByDBusService);

    saveConfig();

    result = id;
}

void Core::removeAction(bool &result, const qulonglong &id)
{
    log(LOG_INFO, "removeAction id:%llu", id);

    QMutexLocker lock(&mDataMutex);

    ShortcutAndActionById::iterator shortcutAndActionById = mShortcutAndActionById.find(id);
    if (shortcutAndActionById == mShortcutAndActionById.end())
    {
        log(LOG_WARNING, "No action registered with id #%llu", id);
        result = false;
        return;
    }

    BaseAction *action = shortcutAndActionById.value().second;

    if (!strcmp(action->type(), DBusAction::id()))
    {
        log(LOG_WARNING, "Cannot unregister DBus action by id");
        result = false;
        return;
    }

    QString shortcut = shortcutAndActionById.value().first;

    X11Shortcut X11shortcut = mX11ByShortcut[shortcut];

    delete action;
    mShortcutAndActionById.erase(shortcutAndActionById);

    IdsByShortcut::iterator idsByShortcut = mIdsByShortcut.find(shortcut);
    if (idsByShortcut != mIdsByShortcut.end())
    {
        idsByShortcut.value().remove(id);
        if (idsByShortcut.value().isEmpty())
        {
            mIdsByShortcut.erase(idsByShortcut);

            if (!remoteXUngrabKey(X11shortcut))
                log(LOG_WARNING, "Cannot ungrab shortcut '%s'", qPrintable(shortcut));
        }
    }

    saveConfig();

    result = true;
}

void Core::setMultipleActionsBehaviour(const MultipleActionsBehaviour &behaviour)
{
    QMutexLocker lock(&mDataMutex);

    mMultipleActionsBehaviour = behaviour;

    saveConfig();
}

void Core::getMultipleActionsBehaviour(MultipleActionsBehaviour &result) const
{
    QMutexLocker lock(&mDataMutex);

    result = mMultipleActionsBehaviour;
}

void Core::getAllActionIds(QList<qulonglong> &result) const
{
    QMutexLocker lock(&mDataMutex);

    result.clear();
    result.reserve(mShortcutAndActionById.size());

    ShortcutAndActionById::const_iterator lastShortcutAndActionById = mShortcutAndActionById.end();
    for (ShortcutAndActionById::const_iterator shortcutAndActionById = mShortcutAndActionById.begin(); shortcutAndActionById != lastShortcutAndActionById; ++shortcutAndActionById)
        result.push_back(shortcutAndActionById.key());
}

GeneralActionInfo Core::actionInfo(const ShortcutAndAction &shortcutAndAction) const
{
    GeneralActionInfo result;

    result.shortcut = shortcutAndAction.first;

    const BaseAction* action = shortcutAndAction.second;

    result.description = action->description();
    result.enabled = action->isEnabled();

    result.type = action->type();

    if (result.type == DBusAction::id())
    {
        const DBusAction *dBusAction = dynamic_cast<const DBusAction*>(action);
        QString service = dBusAction->service();
        PreferredServiceNameByServiceId::const_iterator preferredServiceNameByServiceId = mPreferredServiceNameByServiceId.find(service);
        if (preferredServiceNameByServiceId != mPreferredServiceNameByServiceId.end())
            service = preferredServiceNameByServiceId.value();
        result.info = service + " " + dBusAction->path().path();
    }
    else if (result.type == MethodAction::id())
    {
        const MethodAction *methodAction = dynamic_cast<const MethodAction*>(action);
        result.info = methodAction->service() + " "
            + methodAction->path().path() + " "
            + methodAction->interface() + " "
            + methodAction->method();
    }
    else if (result.type == CommandAction::id())
    {
        const CommandAction *commandAction = dynamic_cast<const CommandAction*>(action);
        result.info = QString("\"") + commandAction->command() + joinToString(commandAction->args(), "\" \"", "\" \"", "\"");
    }

    return result;
}

void Core::getActionById(QPair<bool, GeneralActionInfo> &result, const qulonglong &id) const
{
    log(LOG_INFO, "getActionById id:%llu", id);

    QMutexLocker lock(&mDataMutex);

    ShortcutAndActionById::const_iterator shortcutAndActionById = mShortcutAndActionById.find(id);
    if (shortcutAndActionById == mShortcutAndActionById.end())
    {
        log(LOG_WARNING, "No action registered with id #%llu", id);
        result = qMakePair(false, GeneralActionInfo());
        return;
    }

    result = qMakePair(true, actionInfo(shortcutAndActionById.value()));
}

void Core::getAllActions(QMap<qulonglong,GeneralActionInfo> &result) const
{
    QMutexLocker lock(&mDataMutex);

    result.clear();

    ShortcutAndActionById::const_iterator lastShortcutAndActionById = mShortcutAndActionById.end();
    for (ShortcutAndActionById::const_iterator shortcutAndActionById = mShortcutAndActionById.begin(); shortcutAndActionById != lastShortcutAndActionById; ++shortcutAndActionById)
        result[shortcutAndActionById.key()] = actionInfo(shortcutAndActionById.value());
}

void Core::getDBusActionInfoById(QPair<bool, DBusActionInfo> &result, const qulonglong &id) const
{
    log(LOG_INFO, "getDBusActionInfoById id:%llu", id);

    DBusActionInfo info;

    QMutexLocker lock(&mDataMutex);

    ShortcutAndActionById::const_iterator shortcutAndActionById = mShortcutAndActionById.find(id);
    if (shortcutAndActionById == mShortcutAndActionById.end())
    {
        log(LOG_WARNING, "No action registered with id #%llu", id);
        result = qMakePair(false, info);
        return;
    }

    const BaseAction *action = shortcutAndActionById.value().second;

    if (strcmp(action->type(), DBusAction::id()))
    {
        log(LOG_WARNING, "getDBusActionInfoById attempts to request action of type '%s'", action->type());
        result = qMakePair(false, info);
        return;
    }

    info.shortcut = shortcutAndActionById.value().first;
    info.description = action->description();
    info.enabled = action->isEnabled();

    const DBusAction *dBusAction = dynamic_cast<const DBusAction*>(action);
    info.service = dBusAction->service();
    PreferredServiceNameByServiceId::const_iterator preferredServiceNameByServiceId = mPreferredServiceNameByServiceId.find(info.service);
    if (preferredServiceNameByServiceId != mPreferredServiceNameByServiceId.end())
        info.service = preferredServiceNameByServiceId.value();
    info.path = dBusAction->path();

    result = qMakePair(true, info);
}

void Core::getMethodActionInfoById(QPair<bool, MethodActionInfo> &result, const qulonglong &id) const
{
    log(LOG_INFO, "getMethodActionInfoById id:%llu", id);

    MethodActionInfo info;

    QMutexLocker lock(&mDataMutex);

    ShortcutAndActionById::const_iterator shortcutAndActionById = mShortcutAndActionById.find(id);
    if (shortcutAndActionById == mShortcutAndActionById.end())
    {
        log(LOG_WARNING, "No action registered with id #%llu", id);
        result = qMakePair(false, info);
        return;
    }

    const BaseAction *action = shortcutAndActionById.value().second;

    if (strcmp(action->type(), MethodAction::id()))
    {
        log(LOG_WARNING, "getMethodActionInfoById attempts to request action of type '%s'", action->type());
        result = qMakePair(false, info);
        return;
    }

    info.shortcut = shortcutAndActionById.value().first;
    info.description = action->description();
    info.enabled = action->isEnabled();

    const MethodAction *methodAction = dynamic_cast<const MethodAction*>(action);
    info.service = methodAction->service();
    info.path = methodAction->path();
    info.interface = methodAction->interface();
    info.method = methodAction->method();

    result = qMakePair(true, info);
}

void Core::getCommandActionInfoById(QPair<bool, CommandActionInfo> &result, const qulonglong &id) const
{
    log(LOG_INFO, "getCommandActionInfoById id:%llu", id);

    CommandActionInfo info;

    QMutexLocker lock(&mDataMutex);

    ShortcutAndActionById::const_iterator shortcutAndActionById = mShortcutAndActionById.find(id);
    if (shortcutAndActionById == mShortcutAndActionById.end())
    {
        log(LOG_WARNING, "No action registered with id #%llu", id);
        result = qMakePair(false, info);
        return;
    }

    const BaseAction *action = shortcutAndActionById.value().second;

    if (strcmp(action->type(), CommandAction::id()))
    {
        log(LOG_WARNING, "getCommandActionInfoById attempts to request action of type '%s'", action->type());
        result = qMakePair(false, info);
        return;
    }

    info.shortcut = shortcutAndActionById.value().first;
    info.description = action->description();
    info.enabled = action->isEnabled();

    const CommandAction *commandAction = dynamic_cast<const CommandAction*>(action);
    info.command = commandAction->command();
    info.arguments = commandAction->args();

    result = qMakePair(true, info);
}

void Core::grabShortcut(const uint &timeout, QString &/*shortcut*/, bool &failed, bool &cancelled, bool &timedout, const QDBusMessage &message)
{
    log(LOG_INFO, "grabShortcut timeout:%u", timeout);

    failed = false;
    cancelled = false;
    timedout = false;

    QMutexLocker lock(&mDataMutex);

    if (mGrabbingShortcut)
    {
        failed = true;
        log(LOG_DEBUG, "grabShortcut failed: already grabbing");
        return;
    }

    if ((timeout > 60000) || (timeout < 1000))
    {
        timedout = true;
        log(LOG_DEBUG, "grabShortcut wrong timedout");
        return;
    }

    size_t X11Operation = X11_OP_XGrabKeyboard;
    if (error_t error = writeAll(mX11RequestPipe[STDOUT_FILENO], &X11Operation, sizeof(X11Operation)))
    {
        log(LOG_CRIT, "Cannot write to X11 request pipe: %s", strerror(error));
        qApp->quit();
        return;
    }
    wakeX11Thread();

    int x11result;
    if (error_t error = readAll(mX11ResponsePipe[STDIN_FILENO], &x11result, sizeof(x11result)))
    {
        log(LOG_CRIT, "Cannot read from X11 response pipe: %s", strerror(error));
        qApp->quit();
        return;
    }
    if (x11result)
    {
        failed = true;
        log(LOG_DEBUG, "grabShortcut failed: grab failed");
        return;
    }

    mShortcutGrabRequested = true;

    mShortcutGrabTimeout->setInterval(timeout);
    mShortcutGrabTimeout->start();

    message.setDelayedReply(true);
    mShortcutGrabRequest = message.createReply();

    log(LOG_DEBUG, "grabShortcut delayed");
}

void Core::shortcutGrabbed()
{
    log(LOG_INFO, "shortcutGrabbed");

    QString shortcut;
    bool failed = false;
    bool cancelled = false;
    bool timedout = false;

    QMutexLocker lock(&mDataMutex);

    mShortcutGrabTimeout->stop();

    if (!mShortcutGrabRequested)
        return;

    if (error_t error = readAll(mX11ResponsePipe[STDIN_FILENO], &cancelled, sizeof(cancelled)))
    {
        log(LOG_CRIT, "Cannot read from X11 response pipe: %s", strerror(error));
        qApp->quit();
        return;
    }
    if (!cancelled)
    {
        size_t length;
        if (error_t error = readAll(mX11ResponsePipe[STDIN_FILENO], &length, sizeof(length)))
        {
            log(LOG_CRIT, "Cannot read from X11 response pipe: %s", strerror(error));
            qApp->quit();
            return;
        }
        if (length)
        {
            char *str = new char[length + 1];
            str[length] = '\0';
            if (error_t error = readAll(mX11ResponsePipe[STDIN_FILENO], str, length))
            {
                log(LOG_CRIT, "Cannot read from X11 response pipe: %s", strerror(error));
                qApp->quit();
                return;
            }
            shortcut = str;

            delete[] str;
        }
    }

    if (cancelled)
        log(LOG_DEBUG, "grabShortcut: cancelled");
    else
        log(LOG_DEBUG, "grabShortcut: shortcut:%s", qPrintable(shortcut));

    mShortcutGrabRequest << shortcut << failed << cancelled << timedout;
    QDBusConnection::sessionBus().send(mShortcutGrabRequest);
    mShortcutGrabRequested = false;
}

void Core::shortcutGrabTimedout()
{
    log(LOG_INFO, "shortcutGrabTimedout");

    QString shortcut;
    bool failed = false;
    bool cancelled = false;
    bool timedout = true;

    QMutexLocker lock(&mDataMutex);

    if (!mShortcutGrabRequested)
        return;

    size_t X11Operation = X11_OP_XUngrabKeyboard;
    if (error_t error = writeAll(mX11RequestPipe[STDOUT_FILENO], &X11Operation, sizeof(X11Operation)))
    {
        log(LOG_CRIT, "Cannot write to X11 request pipe: %s", strerror(error));
        qApp->quit();
        return;
    }
    wakeX11Thread();
    char signal;
    if (error_t error = readAll(mX11ResponsePipe[STDIN_FILENO], &signal, sizeof(signal)))
    {
        log(LOG_CRIT, "Cannot read from X11 response pipe: %s", strerror(error));
        qApp->quit();
        return;
    }
    if (signal)
        failed = true;

    log(LOG_DEBUG, "shortcutGrabTimedout: failed:%s", failed ? "true" : "false");

    mShortcutGrabRequest << shortcut << failed << cancelled << timedout;
    QDBusConnection::sessionBus().send(mShortcutGrabRequest);
    mShortcutGrabRequested = false;
}

void Core::cancelShortcutGrab()
{
    log(LOG_INFO, "cancelShortcutGrab");

    QString shortcut;
    bool failed = false;
    bool cancelled = true;
    bool timedout = false;

    QMutexLocker lock(&mDataMutex);

    if (!mGrabbingShortcut)
    {
        log(LOG_DEBUG, "cancelShortcutGrab failed: not grabbing");
        return;
    }

    mShortcutGrabTimeout->stop();

    if (!mShortcutGrabRequested)
        return;

    size_t X11Operation = X11_OP_XUngrabKeyboard;
    if (error_t error = writeAll(mX11RequestPipe[STDOUT_FILENO], &X11Operation, sizeof(X11Operation)))
    {
        log(LOG_CRIT, "Cannot write to X11 request pipe: %s", strerror(error));
        qApp->quit();
        return;
    }
    wakeX11Thread();
    char signal;
    if (error_t error = readAll(mX11ResponsePipe[STDIN_FILENO], &signal, sizeof(signal)))
    {
        log(LOG_CRIT, "Cannot read from X11 response pipe: %s", strerror(error));
        qApp->quit();
        return;
    }
    if (signal)
        failed = true;

    log(LOG_DEBUG, "cancelShortcutGrab: failed:%s", failed ? "true" : "false");

    mShortcutGrabRequest << shortcut << failed << cancelled << timedout;
    QDBusConnection::sessionBus().send(mShortcutGrabRequest);
    mShortcutGrabRequested = false;
}
