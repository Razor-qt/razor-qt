#ifndef RAZORDESKMAN_H
#define RAZORDESKMAN_H

/**
 * @file razordeskman.h
 * @author Christopher "VdoP" Regali
 * @brief declares Razordeskman
*/

#include <QFileSystemWatcher>

#include "razordeskicon.h"
#include "razorworkspace.h"
#include <razorqt/readsettings.h>

typedef QMap<QString,RazorDeskIcon*> IconMap;
typedef QMapIterator<QString,RazorDeskIcon*> IconMapIterator;

/**
 * @brief all the iconManagers are derived from this!
 */
class RazorDeskManager : public QObject
{
    Q_OBJECT
public:
    ~RazorDeskManager();
    RazorDeskManager(RazorWorkSpace* _workspace);

public slots:
    void saveIconState();
    virtual void updateIconList() = 0;

protected:
    void restoreIconState();
    RazorWorkSpace* workSpace()
    {
        return workspace;
    }

    IconMap m_iconList;

private:
    ReadSettings *deskicons;
    RazorWorkSpace * workspace;

    QFileSystemWatcher * m_fsw;
};



#endif

