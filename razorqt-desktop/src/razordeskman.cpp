#ifndef RAZORDESKMAN_CPP
#define RAZORDESKMAN_CPP
#include "razordeskman.h"
QList< Razordeskicondata* >* Razordeskmanager::iconList()
{
  return NULL;
}

void Razordeskmanager::saveIconState()
{
      qDebug() << priviconList;
  for (int i = 0; i < priviconList.count(); i++)
  {
    QString x,y;
    x.setNum(priviconList.at(i)->getPosition().x());
    y.setNum(priviconList.at(i)->getPosition().y());
    QString pos = x + "|" + y;

    qDebug() << pos;
    deskicons->setValue(priviconList.at(i)->text(),pos);
  }
  deskicons->debugSettings();
  deskicons->safeSettings();
}

void Razordeskmanager::showIcons()
{

}

Razordeskmanager::~Razordeskmanager()
{
  for (int i = 0; i < priviconList.count(); i ++)
  {
    delete priviconList.at(i);
    priviconList.removeAt(i);
  }
  delete deskicons;
}



void Razordeskmanager::updateIconList()
{

}

void Razordeskmanager::restoreIconState()
{
  qDebug() << "restoring icon state!";
  for (int i = 0; i < priviconList.count(); i ++)
  {
    if (deskicons->getValue(priviconList.at(i)->text()) != "")
    {
      qDebug() << "found saved position for: " << priviconList.at(i) << " value: " << deskicons->getValue(priviconList.at(i)->text());
      QStringList explode = deskicons->getValue(priviconList.at(i)->text()).split("|");
      QPoint npos(explode.at(0).toInt(), explode.at(1).toInt());
      priviconList.at(i)->setPos(npos);
    }
  }
  qDebug() << "restoring done";
}



Razordeskmanager::Razordeskmanager(Razorworkspace* _workspace)
{
  qDebug() << "Initializing!!";
  workspace = _workspace;
  deskicons=new Readsettings("deskicons.conf");
  
}



#endif
