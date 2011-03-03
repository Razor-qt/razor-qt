#ifndef RAZORDESKMAN_CPP
#define RAZORDESKMAN_CPP
#include <QDesktopServices>
#include "razordeskman.h"


void RazorDeskManager::saveIconState()
{
    qDebug() << "saveIconState" << m_iconList;
    QSettings * s = deskicons->settings();
    s->beginWriteArray("icons");
    IconMapIterator i(m_iconList);
    int ix = 0;
    while (i.hasNext())
    {
        i.next();
        s->setArrayIndex(ix);
        s->setValue("name", i.key());
        s->setValue("point", i.value()->pos());
        ++ix;
    }
    s->endArray();
}


RazorDeskManager::~RazorDeskManager()
{
    IconMapIterator i(m_iconList);
    while (i.hasNext())
    {
        i.next();
        delete i.value();
    }
    m_iconList.clear();
}


void RazorDeskManager::restoreIconState()
{
    qDebug() << "restoring icon state!";

    QSettings * s = deskicons->settings();
    int count = s->beginReadArray("icons");

    QString name;
    for (int i = 0; i < count; ++i)
    {
        s->setArrayIndex(i);
        name = s->value("name", "").toString();
        if (! m_iconList.contains(name))
        {
            qDebug() << "Icon" << name << "is not found. It's strange but not critical";
            continue;
        }
        m_iconList[name]->setPos(s->value("point").value<QPoint>());
    }
    s->endArray();

    qDebug() << "restoring done";
}



RazorDeskManager::RazorDeskManager(RazorWorkSpace* _workspace)
{
    qDebug() << "Initializing!!" << QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
    workspace = _workspace;
    deskicons = new ReadSettings("deskicons", this);
    
    m_fsw = new QFileSystemWatcher(QStringList() << QDesktopServices::storageLocation(QDesktopServices::DesktopLocation), this);
    connect(m_fsw, SIGNAL(directoryChanged(const QString&)), this, SLOT(updateIconList()));
}



#endif
