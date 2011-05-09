#ifndef RAZORDESKMAN_H
#define RAZORDESKMAN_H

/**
 * @file razordeskman.h
 * @author Christopher "VdoP" Regali
 * @brief declares Razordeskman
*/

#include <QFileSystemWatcher>

#include "razordeskicon.h"
#include <razorqt/readsettings.h>
#include <desktopplugin.h>

typedef QMap<QString,RazorDeskIconBase*> IconMap;
typedef QMapIterator<QString,RazorDeskIconBase*> IconMapIterator;

EXPORT_RAZOR_DESKTOP_PLUGIN_H

/**
 * @brief all the iconManagers are derived from this!
 */
class RazorDeskManager : public QObject, public DesktopPlugin
{
    Q_OBJECT
public:
    RazorDeskManager(const QString & configId, ReadSettings * config, ReadTheme * theme);
    ~RazorDeskManager();

    QString info();

public slots:
    void saveIconState();
    void updateIconList();

private:
    void restoreIconState();

    IconMap m_iconList;
    QFileSystemWatcher * m_fsw;

    ReadSettings *deskicons;
};



#endif

