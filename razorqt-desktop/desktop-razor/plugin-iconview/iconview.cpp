#include "iconview.h"
#include "iconscene.h"
#include <QtDebug>
#include <QDesktopServices>
#include <QGraphicsView>
#include <QPropertyAnimation>


EXPORT_RAZOR_DESKTOP_WIDGET_PLUGIN_CPP(IconView)


IconView::IconView(QGraphicsScene * scene, const QString & configId, ReadSettings * config)
    : DesktopWidgetPlugin(scene, configId, config)
{
    setObjectName("IconView");

    QSettings * s = config->settings();
    s->beginGroup(configId);

    QString dir = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
    dir = s->value("directory", dir).toString();
    qDebug() << "CFG: " << dir;
    s->endGroup();

    // Hack to ensure the fully transparent QGraphicsView background
    QPalette palette;
    palette.setBrush(QPalette::Base, Qt::NoBrush);
    setPalette(palette);
    // Required to display wallpaper
    setAttribute(Qt::WA_TranslucentBackground);
    // no border at all finally
    setFrameShape(QFrame::NoFrame);
    
    m_scene = new IconScene(dir);
   
    setScene(m_scene);
    
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::TextAntialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setRenderHint(QPainter::HighQualityAntialiasing);
    
    setDragMode(QGraphicsView::RubberBandDrag);
    
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    
    setCacheMode(QGraphicsView::CacheBackground);
    
    setAlignment(Qt::AlignTop | Qt::AlignLeft);
    
    // "cool" display effect
    setWindowOpacity(0.0);
}

IconView::~IconView()
{
    delete m_scene;
}

    
QString IconView::info()
{
    return QObject::tr("Display content of the given directory/folder");
}

QString IconView::instanceInfo()
{
    return QObject::tr("Icon View:") + " " + m_configId;
}

void IconView::setSizeAndPosition(const QPointF & position, const QSizeF & size)
{
    qDebug() << "Moving to" << position << "resizing" << size;
    move(position.x(), position.y());
    resize(size.width(), size.height());
    m_scene->setParentSize(size);
    
    // "cool" display FX - the main part
    QPropertyAnimation * animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(3000);
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->start();
}

void IconView::save()
{
    QSettings *s = m_config->settings();
    s->beginGroup(m_configId);
    s->setValue("x", pos().x());
    s->setValue("y", pos().y());
    s->setValue("w", size().width());
    s->setValue("h", size().height());
    s->endGroup();
}
