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
class Razordeskmanager : public QObject
{
  Q_OBJECT
public:
    ~Razordeskmanager();
    Razordeskmanager(Razorworkspace* _workspace);
    virtual void updateIconList();
    virtual void showIcons();
    virtual QList<Razordeskicondata*>* iconList();
    void restoreIconState();
    Razorworkspace* workSpace() {return workspace;}
    QList<Razordeskicondata*>* privIconList() {return &priviconList;}
    
public slots:
  void saveIconState();
  
private:
  Readsettings *deskicons;
  Razorworkspace * workspace;
  QList<Razordeskicondata*> priviconList;
};



#endif

