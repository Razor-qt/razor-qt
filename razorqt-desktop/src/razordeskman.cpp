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

    // Store positions of already setPos-ed icons here temporarily - used in calculation below.
    QList<QRect> positionFrames;

    // First - try to move user moved icons to saved location
    QString name;
    for (int i = 0; i < count; ++i)
    {
        s->setArrayIndex(i);
        name = s->value("name", "").toString();
        if (! m_iconList.contains(name))
        {
            continue;
        }

        RazorDeskIconBase* icon = m_iconList[name];
        QPoint p = s->value("point").value<QPoint>();
        icon->setPos(p);
        positionFrames.append(QRect(p.x(), p.y(), icon->width(), icon->height()));
    }
    s->endArray();
    
    // Then take all remaining icons (QPoint(0.0)) and try to calculate
    // their position.
    
    IconMapIterator it(m_iconList);
    QDesktopWidget *dw = QApplication::desktop();
    int xMax = dw->availableGeometry().width() - 140;
    int yMax = dw->availableGeometry().height();
    // initial placement to be tried
    int x = 10;
    int y = 10;
    // those 70's are taken from RazorDeskIconBase:paintEvent. Ugly, but it's fine for now.
    int w = 70;
    int h = 70;
    while (it.hasNext())
    {
        it.next();
        // it's moved already to its saved position
        if (it.value()->pos() != QPoint(0,0))
            continue;
        // 
        QRect newPos(x, y, w, h);
        foreach (QRect r, positionFrames)
        {
            if (!r.intersects(newPos))
                continue;

            y += 80;
            if (y > yMax)
            {
                y = 10;
                x += 80;
                if (x > xMax)
                {
                    // TODO/FIXME: better failsafe...
                    it.value()->setPos(QPoint(1, 1));
                    qDebug() << "Position boundaries reached. Ite means there is no free space on screen. Saving at 1,1...";
                }
            }
            newPos = QRect(x, y, w, h);
        } // foreach positionFrames
        
        //qDebug() << "Position from calculation (free space)" << newPos;
        positionFrames.append(newPos);
        it.value()->setPos(newPos.topLeft());

    } // while it

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

