#include "iconview.h"
#include "iconscene.h"
#include <QtDebug>
#include <QDesktopServices>
#include <QGraphicsView>
#include <QGraphicsGridLayout>


EXPORT_RAZOR_DESKTOP_WIDGET_PLUGIN_CPP(IconView)


IconView::IconView(QGraphicsScene * scene, const QString & configId, ReadSettings * config)
    : DesktopWidgetPlugin(scene, configId, config)
{
    setObjectName("IconView");

    QSettings * s = config->settings();
    s->beginGroup(configId);

    QString dir = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
    dir = s->value("directory", dir).toString();
    s->endGroup();
    
    setAttribute(Qt::WA_TranslucentBackground);
    
    m_scene = new IconScene(dir);
   
    setScene(m_scene);
    
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::TextAntialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setRenderHint(QPainter::HighQualityAntialiasing);
    
    setDragMode(QGraphicsView::RubberBandDrag);
    
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    
    setCacheMode(QGraphicsView::CacheBackground);
    setBackgroundBrush(Qt::NoBrush);
}

IconView::~IconView()
{
    delete m_scene;
}

    
QString IconView::info()
{
    return QObject::tr("Display content of the given directory/folder");
}

void IconView::setSizeAndPosition(const QPointF & position, const QSizeF & size)
{
    qDebug() << "Moving to" << position << "resizing" << size;
    move(position.x(), position.y());
    resize(size.width(), size.height());
    //m_scene->setSceneRect(QRectF(position, size));
}
