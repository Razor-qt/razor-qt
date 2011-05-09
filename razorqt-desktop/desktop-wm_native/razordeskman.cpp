#include <QApplication>
#include <QString>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QDirIterator>
#include <QVariant>
#include <QtDebug>

#include "razorqt/xfitman.h"

#include "razordeskman.h"


EXPORT_RAZOR_DESKTOP_PLUGIN_CPP(RazorDeskManager);


QString RazorDeskManager::info()
{
    return tr("A legacy desktop manager. "
              "It allows to use used window manager's native menues. "
              "Only  classical icons and simple wallpaper functionality is implemented"
            );
}

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



RazorDeskManager::RazorDeskManager(const QString & configId, ReadSettings * config, ReadTheme * theme)
    : DesktopPlugin(configId, config, theme)
{
    config->settings()->beginGroup(configId);
    bool makeIcons = config->settings()->value("icons", false).toBool();
    //now we got the desktop we need to determine if the user wants a defined picture there
    QString finalPixmap = config->settings()->value("wallpaper", "").toString();
    config->settings()->endGroup();

    if (finalPixmap.isEmpty())
    {
        //now we want to use the system default - we still need to find that one out though
        finalPixmap = theme->desktopBackground();
        qDebug() << "trying to get system-defaults" << finalPixmap;
    }

    qDebug() << finalPixmap;
    //now try to set it - if its not set we assume the theme stylesheet takes care of it some way (like a gradient or stuff)
    if (! finalPixmap.isEmpty())
    {
        qDebug() << "Creating icons";
        int width,height;
        QDesktopWidget * dw = QApplication::desktop();
        if (dw->screenCount() == 1)
        {
            width=dw->width();
            height = dw->height();
        }
        else
        {
            width=dw->screenGeometry(-1).width();
            height=dw->screenGeometry(-1).height();
        }
        xfitMan().setRootBackground(((QPixmap) finalPixmap).scaled(width,height));
    }

    
    if (makeIcons)
    {
        deskicons = new ReadSettings("deskicons", this);
        updateIconList();
    
        m_fsw = new QFileSystemWatcher(QStringList() << QDesktopServices::storageLocation(QDesktopServices::DesktopLocation), this);
        connect(m_fsw, SIGNAL(directoryChanged(const QString&)), this, SLOT(updateIconList()));
    }
}

void RazorDeskManager::updateIconList()
{
    m_fsw->blockSignals(true);

    qDebug() << "updateIconList";
    QDirIterator dirIter(QDesktopServices::storageLocation(QDesktopServices::DesktopLocation));

    QStringList tmpList;

    while (dirIter.hasNext())
    {
        dirIter.next();
        QString df(dirIter.filePath());

        // HACK: QDir::NoDotAndDotDot does not work so this fixes it...
        if (df.endsWith("/..") || df.endsWith("/."))
            continue;

        qDebug() << df;
        tmpList.append(df);

        // only non existing icons are created
        if (m_iconList.contains(df))
        {
            qDebug() << "updateIconList REREAD. Skip:" << df;
            continue;
        }

        QPoint pos(0, 0);
        RazorDeskIconBase * idata;

        if (dirIter.filePath().endsWith(".desktop")) //only use .desktop files!
        {
            XdgDesktopFile* tmp = new XdgDesktopFile(df);

            if (tmp->isShow())
            {
                idata = new RazorDeskIconDesktop(tmp, pos);
            }
            else
            {
                delete tmp;
                qDebug() << "Desktop file" << df << "isShow==false";
                continue;
            }
        }
        else
        {
            idata = new RazorDeskIconFile(df, pos);
        }

        connect(idata, SIGNAL(moved(QPoint)), this, SLOT(saveIconState()));
        m_iconList[df] = idata;
    }

    // now remove potentialy deleted icons
    IconMapIterator iter(m_iconList);
    while (iter.hasNext())
    {
        iter.next();
        if (tmpList.contains(iter.key()))
            continue;
        delete m_iconList.take(iter.key());
    }

    qDebug() << "Razordeskmanl: found " << m_iconList.count() << " usable desktop-entries";

    restoreIconState();
    m_fsw->blockSignals(false);
}
