#include "iconview.h"
#include "iconscene.h"
#include <QtDebug>
#include <QDesktopServices>
#include <QGraphicsView>
#include <QGraphicsGridLayout>


EXPORT_RAZOR_DESKTOP_WIDGET_PLUGIN_CPP(IconView)


IconView::IconView(const QString & configId, ReadSettings * config)
    : DesktopWidgetPlugin(configId, config)
{
    QSettings * s = config->settings();
    s->beginGroup(configId);

    QString dir = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
    dir = s->value("directory", dir).toString();
    s->endGroup();

#if 0      
    QGraphicsGridLayout * layout = new QGraphicsGridLayout();
    layout->addItem(new QGraphicsTextItem(dir), 0, 0);

  
    // TODO/FIXME: delete!
    m_scene = new IconScene(dir);
   
    // TODO/FIXME: delete!
    QGraphicsView * view = new QGraphicsView(m_scene);
    
    view->setRenderHint(QPainter::Antialiasing);
    view->setRenderHint(QPainter::TextAntialiasing);
    view->setRenderHint(QPainter::SmoothPixmapTransform);
    view->setRenderHint(QPainter::HighQualityAntialiasing);
    
    view->setDragMode(QGraphicsView::RubberBandDrag);
    
    view->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    
    view->setCacheMode(QGraphicsView::CacheBackground);
    view->setBackgroundBrush(Qt::NoBrush);

    layout->addWidget(view);

    setLayout(layout);    
#endif
}

IconView::~IconView()
{
}

    
QString IconView::info()
{
    return QObject::tr("Display content of the given directory/folder");
}

void IconView::setSizeAndPosition(const QPointF & position, const QSizeF & size)
{
    qDebug() << "Moving to" << position << "resizing" << size;
    setPos(position);
    resize(size);
    //m_scene->setSceneRect(QRectF(position, size));
}
