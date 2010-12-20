#ifndef RAZORDESKMAN_CPP
#define RAZORDESKMAN_CPP
#include "razordeskman.h"
QList< RazorDeskIconData* >* RazorDeskManager::iconList()
{
    return NULL;
}

void RazorDeskManager::saveIconState()
{
    qDebug() << priviconList;
    for (int i = 0; i < priviconList.count(); i++)
    {
        QString x,y;
        x.setNum(priviconList.at(i)->getPosition().x());
        y.setNum(priviconList.at(i)->getPosition().y());
        QString pos = x + "|" + y;

        qDebug() << pos;
        //deskicons->setValue(priviconList.at(i)->text(),pos);
    }
    //deskicons->debugSettings();
    //deskicons->saveSettings();
}

void RazorDeskManager::showIcons()
{

}

RazorDeskManager::~RazorDeskManager()
{
    for (int i = 0; i < priviconList.count(); i ++)
    {
        delete priviconList.at(i);
        priviconList.removeAt(i);
    }
    delete deskicons;
}



void RazorDeskManager::updateIconList()
{

}

void RazorDeskManager::restoreIconState()
{
    qDebug() << "restoring icon state!";
    for (int i = 0; i < priviconList.count(); i ++)
    {
#if 0
        if (deskicons->getString(priviconList.at(i)->text()) != "")
        {
            qDebug() << "found saved position for: " << priviconList.at(i) << " value: " << deskicons->getString(priviconList.at(i)->text());
            QStringList explode = deskicons->getString(priviconList.at(i)->text()).split("|");
            QPoint npos(explode.at(0).toInt(), explode.at(1).toInt());
            priviconList.at(i)->setPos(npos);
        }
#endif
    }
    qDebug() << "restoring done";
}



RazorDeskManager::RazorDeskManager(RazorWorkSpace* _workspace)
{
    qDebug() << "Initializing!!";
    workspace = _workspace;
    deskicons = new ReadSettings("deskicons");
}



#endif
