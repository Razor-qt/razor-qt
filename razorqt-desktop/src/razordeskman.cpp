#ifndef RAZORDESKMAN_CPP
#define RAZORDESKMAN_CPP
#include "razordeskman.h"


void RazorDeskManager::saveIconState()
{
    qDebug() << "saveIconState" << privIconList;
    QSettings * s = deskicons->settings();
    s->beginWriteArray("icons");
    for (int i = 0; i < privIconList.count(); i++)
    {
        s->setArrayIndex(i);
        s->setValue("name", privIconList.at(i)->text());
        s->setValue("point", privIconList.at(i)->pos());
    }
    s->endArray();
}


RazorDeskManager::~RazorDeskManager()
{
    for (int i = 0; i < privIconList.count(); i ++)
    {
        delete privIconList.at(i);
        privIconList.removeAt(i);
    }
}


void RazorDeskManager::restoreIconState()
{
    qDebug() << "restoring icon state!";
    // map icon name to its position. It's used to restore position
    // later with privIconList merge
    QMap<QString,QPoint> positions;

    QSettings * s = deskicons->settings();
    int count = s->beginReadArray("icons");

    for (int i = 0; i < count; ++i)
    {
        s->setArrayIndex(i);
        positions[s->value("name", "").toString()] = s->value("point").value<QPoint>();
    }
    s->endArray();

    for (int i = 0; i < privIconList.count(); i ++)
    {
        if (! positions.contains(privIconList.at(i)->text()) )
            continue;

        qDebug() << "found saved position for: " << privIconList.at(i);
        privIconList.at(i)->setPos(positions[privIconList.at(i)->text()]);
    }
    qDebug() << "restoring done";
}



RazorDeskManager::RazorDeskManager(RazorWorkSpace* _workspace)
{
    qDebug() << "Initializing!!";
    workspace = _workspace;
    deskicons = new ReadSettings("deskicons", this);
}



#endif
