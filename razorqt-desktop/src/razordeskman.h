#ifndef RAZORDESKMAN_H
#define RAZORDESKMAN_H

/**
 * @file razordeskman.h
 * @author Christopher "VdoP" Regali
 * @brief declares Razordeskman
 */

#include "razordeskicon.h"
#include "razorworkspace.h"
#include <razorqt/readsettings.h>
/**
 * @brief all the iconManagers are derived from this!
 */
class RazorDeskManager : public QObject
{
    Q_OBJECT
public:
    ~RazorDeskManager();
    RazorDeskManager(RazorWorkSpace* _workspace);
    virtual void updateIconList();
    virtual void showIcons();
    virtual QList<RazorDeskIconData*>* iconList();
    void restoreIconState();
    RazorWorkSpace* workSpace()
    {
        return workspace;
    }
    QList<RazorDeskIconData*>* privIconList()
    {
        return &priviconList;
    }

public slots:
    void saveIconState();

private:
    ReadSettings *deskicons;
    RazorWorkSpace * workspace;
    QList<RazorDeskIconData*> priviconList;
};



#endif

