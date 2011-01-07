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

public slots:
    void saveIconState();

protected:
    virtual void updateIconList() = 0;
    virtual QList<RazorDeskIconData*> iconList() = 0;
    void restoreIconState();
    RazorWorkSpace* workSpace()
    {
        return workspace;
    }

    QList<RazorDeskIconData*> privIconList;

private:
    ReadSettings *deskicons;
    RazorWorkSpace * workspace;
};



#endif

