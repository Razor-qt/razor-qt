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

#ifndef GLOBAL_ACTION_DAEMON__CORE__INCLUDED
#define GLOBAL_ACTION_DAEMON__CORE__INCLUDED


#include <QThread>
#include <QMap>
#include <QSet>
#include <QString>
#include <QQueue>
#include <QMutex>
#include <QList>
#include <QPair>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusObjectPath>

#include "meta_types.hpp"
#include "log_target.hpp"

extern "C" {
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xproto.h>
#undef Bool
}


class QTimer;
class DaemonAdaptor;
class NativeAdaptor;
class DBusProxy;
class BaseAction;

template<class Key>
class QOrderedSet : public QMap<Key, Key>
{
public:
    typename QMap<Key, Key>::iterator insert(const Key &akey)
    {
        return QMap<Key, Key>::insert(akey, akey);
    }
};

class Core : public QThread, public LogTarget
{
    Q_OBJECT
public:
    Core(bool useSyslog, bool minLogLevelSet, int minLogLevel, const QStringList &configFiles, bool multipleActionsBehaviourSet, MultipleActionsBehaviour multipleActionsBehaviour, QObject * parent = 0);
    ~Core();

    bool ready() const { return mReady; }

    virtual void log(int level, const char *format, ...) const;

signals:
    void onShortcutGrabbed();

private:
    Core(const Core &);
    Core& operator = (const Core &);

private:
    typedef QPair<KeyCode, unsigned int> X11Shortcut;
    typedef QMap<X11Shortcut, QString> ShortcutByX11;
    typedef QMap<QString, X11Shortcut> X11ByShortcut;
    typedef QOrderedSet<qulonglong> Ids;
    typedef QMap<QString, Ids> IdsByShortcut;
    typedef QPair<QString, QDBusObjectPath> DBusClient;
    typedef QMap<DBusClient, qulonglong> IdByDBusClient;
    typedef QOrderedSet<QDBusObjectPath> DBusPaths;
    typedef QMap<QString, DBusPaths> DBusPathsByDBusService;
    typedef QPair<QString, BaseAction*> ShortcutAndAction;
    typedef QMap<qulonglong, ShortcutAndAction> ShortcutAndActionById;
    typedef QOrderedSet<QString> ServiceNames;
    typedef QMap<QString, ServiceNames> ServiceNamesByServiceId;
    typedef QMap<QString, QString> PreferredServiceNameByServiceId;
    typedef QMap<QString, QString> ServiceIdByServiceName;

private slots:
    void serviceAppeared(const QString &service, const QString &id);
    void serviceDisappeared(const QString &service, const QString &id);

    void addDBusAction(QPair<QString, qulonglong> &result, const QString &shortcut, const QDBusObjectPath &path, const QString &description, const QString &sender);
    void addMethodAction(QPair<QString, qulonglong> &result, const QString &shortcut, const QString &service, const QDBusObjectPath &path, const QString &interface, const QString &method, const QString &description);
    void addCommandAction(QPair<QString, qulonglong> &result, const QString &shortcut, const QString &command, const QStringList &arguments, const QString &description);

    void modifyDBusAction(qulonglong &result, const QDBusObjectPath &path, const QString &description, const QString &sender);
    void modifyActionDescription(bool &result, const qulonglong &id, const QString &description);
    void modifyMethodAction(bool &result, const qulonglong &id, const QString &service, const QDBusObjectPath &path, const QString &interface, const QString &method, const QString &description);
    void modifyCommandAction(bool &result, const qulonglong &id, const QString &command, const QStringList &arguments, const QString &description);

    void enableDBusAction(bool &result, const QDBusObjectPath &path, bool enabled, const QString &sender);
    void isDBusActionEnabled(bool &enabled, const QDBusObjectPath &path, const QString &sender);
    void enableAction(bool &result, qulonglong id, bool enabled);
    void isActionEnabled(bool &enabled, qulonglong id);

    void changeDBusShortcut(QPair<QString, qulonglong> &result, const QDBusObjectPath &path, const QString &shortcut, const QString &sender);
    void changeShortcut(QString &result, const qulonglong &id, const QString &shortcut);

    void swapActions(bool &result, const qulonglong &id1, const qulonglong &id2);

    void removeDBusAction(qulonglong &result, const QDBusObjectPath &path, const QString &sender);
    void removeAction(bool &result, const qulonglong &id);

    void setMultipleActionsBehaviour(const MultipleActionsBehaviour &behaviour);
    void getMultipleActionsBehaviour(MultipleActionsBehaviour &result) const;

    void getAllActionIds(QList<qulonglong> &result) const;
    void getActionById(QPair<bool, GeneralActionInfo> &result, const qulonglong &id) const;
    void getAllActions(QMap<qulonglong,GeneralActionInfo> &result) const;

    void getDBusActionInfoById(QPair<bool, DBusActionInfo> &result, const qulonglong &id) const;
    void getMethodActionInfoById(QPair<bool, MethodActionInfo> &result, const qulonglong &id) const;
    void getCommandActionInfoById(QPair<bool, CommandActionInfo> &result, const qulonglong &id) const;

    void grabShortcut(const uint &timeout, QString &shortcut, bool &failed, bool &cancelled, bool &timedout, const QDBusMessage &message);
    void cancelShortcutGrab();

    void shortcutGrabbed();
    void shortcutGrabTimedout();

private:
    QPair<QString, qulonglong> addOrRegisterDBusAction(const QString &shortcut, const QString &service, const QDBusObjectPath &path, const QString &description, const QString &sender);
    qulonglong registerDBusAction(const QString &shortcut, const QString &service, const QDBusObjectPath &path, const QString &description);
    qulonglong registerMethodAction(const QString &shortcut, const QString &service, const QDBusObjectPath &path, const QString &interface, const QString &method, const QString &description);
    qulonglong registerCommandAction(const QString &shortcut, const QString &command, const QStringList &arguments, const QString &description);

    GeneralActionInfo actionInfo(const ShortcutAndAction &shortcutAndAction) const;

    friend void unixSignalHandler(int signalNumber);
    void unixSignalHandler(int signalNumber);

    friend int x11ErrorHandler(Display *display, XErrorEvent *errorEvent);
    int x11ErrorHandler(Display *display, XErrorEvent *errorEvent);

    X11Shortcut ShortcutToX11(const QString &shortcut);
    QString X11ToShortcut(const X11Shortcut &X11shortcut);

    void wakeX11Thread();

    void run();

    KeyCode remoteStringToKeycode(const QString &str);
    QString remoteKeycodeToString(KeyCode keyCode);
    bool remoteXGrabKey(const X11Shortcut &X11shortcut);
    bool remoteXUngrabKey(const X11Shortcut &X11shortcut);

    QString grabOrReuseKey(const X11Shortcut &X11shortcut, const QString &shortcut);

    bool addActionCommon(const QString &shortcut, X11Shortcut &X11shortcut, QString &usedShortcut);

    bool isEscape(KeySym keySym, unsigned int modifiers);
    bool isModifier(KeySym keySym);
    bool isAllowed(KeySym keySym, unsigned int modifiers);

    void saveConfig();

    void lockX11Error();
    bool checkX11Error(int level = LOG_NOTICE, uint timeout = 10);

    bool waitForX11Error(int level, uint timeout);

private:
    bool mReady;
    bool mUseSyslog;

    int mMinLogLevel;

    int mX11ErrorPipe[2];
    int mX11RequestPipe[2];
    int mX11ResponsePipe[2];
    Display *mDisplay;
    Window mInterClientCommunicationWindow;
    bool mX11EventLoopActive;

    mutable QMutex mX11ErrorMutex;

    QDBusConnection *mSessionConnection;
    DaemonAdaptor *mDaemonAdaptor;
    NativeAdaptor *mNativeAdaptor;
    DBusProxy *mDBusProxy;

    mutable QMutex mDataMutex;

    qulonglong mLastId;

    bool mGrabbingShortcut;

    X11ByShortcut mX11ByShortcut;
    ShortcutByX11 mShortcutByX11;
    IdsByShortcut mIdsByShortcut;
    IdByDBusClient mIdByDBusClient;
    DBusPathsByDBusService mDBusPathsByDBusService;
    ShortcutAndActionById mShortcutAndActionById;
    ServiceNamesByServiceId mServiceNamesByServiceId;
    PreferredServiceNameByServiceId mPreferredServiceNameByServiceId;
    ServiceIdByServiceName mServiceIdByServiceName;


    unsigned int NumLockMask;
    unsigned int ScrollLockMask;
    unsigned int CapsLockMask;
    unsigned int AltMask;
    unsigned int MetaMask;
    unsigned int Level3Mask;
    unsigned int Level5Mask;

    MultipleActionsBehaviour mMultipleActionsBehaviour;

    bool mAllowGrabLocks;
    bool mAllowGrabBaseSpecial;
    bool mAllowGrabMiscSpecial;
    bool mAllowGrabBaseKeypad;
    bool mAllowGrabMiscKeypad;
    bool mAllowGrabPrintable;

    QString mConfigFile;
    bool mSaveAllowed;

    QTimer *mShortcutGrabTimeout;
    QDBusMessage mShortcutGrabRequest;
    bool mShortcutGrabRequested;

    bool mSuppressX11ErrorMessages;
};

#endif // GLOBAL_ACTION_DAEMON__CORE__INCLUDED
