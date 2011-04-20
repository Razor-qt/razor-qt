#include "iconscene.h"
#include "desktopicon.h"
#include <QDesktopServices>
#include <QDir>
#include <QFileSystemWatcher>
#include <QFileInfoList>
#include <QtDebug>


IconScene::IconScene(const QString & directory, QObject * parent)
    : QGraphicsScene(parent)
{
    // TODO/FIXME: Transparent! Now it's debugging blue
    //setBackgroundBrush(QColor("#050d4e"));

    QStringList dirs;
    if (QDir(directory).exists())
        dirs << directory;
    else
    {
        qDebug() << "ERROR config dir" << directory << "does not exist";
        dirs << QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
    }

    m_fsw = new QFileSystemWatcher(dirs, this);
    connect(m_fsw, SIGNAL(directoryChanged(const QString&)), this, SLOT(updateIconList()));
}

void IconScene::updateIconList()
{
    qDebug() << "IconScene::updateIconList";
  
    m_fsw->blockSignals(true);

    // bruteforce cleanup
    foreach (QGraphicsItem* item, items())
    {
        removeItem(item);
        delete item;
    }
    
    //QDirIterator dirIter(m_fsw->directories().at(0));
    QDir d(m_fsw->directories().at(0));
    int x = 30;
    int y = 10;

    //while (dirIter.hasNext())
    foreach (QFileInfo dirIter, d.entryInfoList(QDir::NoDotAndDotDot|QDir::AllEntries, QDir::DirsFirst|QDir::Name|QDir::IgnoreCase ))
    {
//        dirIter.next();
        QString df(dirIter.filePath());

        // HACK: QDir::NoDotAndDotDot does not work so this fixes it...
        if (df.endsWith("/..") || df.endsWith("/."))
            continue;

        //qDebug() << df;
        IconBase * idata = 0;

        if (dirIter.filePath().endsWith(".desktop")) //only use .desktop files!
        {
            XdgDesktopFile* tmp = new XdgDesktopFile(df);

            if (tmp->isShow())
            {
                idata = new DesktopIcon(tmp);
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
            idata = new FileIcon(df);
        }

        if (idata)
        {
            //qDebug() << "   POSITIONING" << x << y;
            addItem(idata);
            idata->setPos(x, y);
            while (idata->collidingItems().count())
            {
                idata->setPos(x, y*IconBase::iconHeight());
                y += IconBase::iconHeight() + 20;
            }
            y += IconBase::iconHeight() + 20;
            
            if (y > m_parentSize.height() - IconBase::iconHeight()) //IconBase::iconHeight() simulates the slider. Ugly, I know...
            {
                x = x + IconBase::iconWidth() + 20;
                y = 10;
            }
        }
    }

    m_fsw->blockSignals(false);
}

void IconScene::setParentSize(const QSizeF & size)
{
    qDebug() << "IconScene::setParentSize" << size;
    m_parentSize = size;
    updateIconList();
}

bool IconScene::blockGlobalMenu()
{
    qDebug() << "bool IconScene::blockGlobalMenu()" << mouseGrabberItem();

    IconBase * item = dynamic_cast<IconBase*>(mouseGrabberItem());
    return item != 0;
}
