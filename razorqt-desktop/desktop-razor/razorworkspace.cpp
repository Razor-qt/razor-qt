#include <QApplication>
#include <QDesktopWidget>
#include <QLibrary>

#include "razorworkspace.h"
#include "workspacemanager.h"
#include "desktopwidgetplugin.h"
#include <razorqt/readsettings.h>

#include <QtDebug>
#include <QGraphicsTextItem>

RazorWorkSpace::RazorWorkSpace(ReadSettings * config, int screen, QWidget* parent)
    : QGraphicsView(parent),
      m_config(config)
{
    qDebug() << "RazorWorkSpace::RazorWorkSpace";
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnBottomHint);
    setAttribute(Qt::WA_X11NetWmWindowTypeDesktop);

    QRect geometry(QApplication::desktop()->availableGeometry(screen));
    
    move(geometry.x(), geometry.y());
    resize(geometry.width(), geometry.height());
    //setMaximumSize(geometry.width(), geometry.height());
    
    m_scene = new QGraphicsScene(geometry.x(), geometry.y(), geometry.width(), geometry.height(), this);
    setScene(m_scene);
    
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::TextAntialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setRenderHint(QPainter::HighQualityAntialiasing);
    
    setDragMode(QGraphicsView::RubberBandDrag);
    
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    
    setCacheMode(QGraphicsView::CacheBackground);
}

void RazorWorkSpace::setConfig(const WorkspaceConfig & bg)
{
    QBrush background;
    switch (bg.wallpaperType)
    {
        case (RazorWorkSpaceManager::BackgroundPixmap):
        {
            QPixmap pm(bg.wallpaper);
            if (pm.isNull())
                // TODO/FIXME: defaults
                qDebug() << "Pixmap is null" << bg.wallpaper;

            pm = pm.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            background.setTexture(pm);
            //        finalPixmap = Razor::getInstance().themesettings()->desktopBackground();
            break;
        }
        default:
            background.setColor(QColor(bg.wallpaper));
            background.setStyle(Qt::SolidPattern);
    }
    setBackgroundBrush(background);
    
    // load plugins
    
    foreach (QString configId, bg.plugins)
    {
        qDebug() << "Plugin conf id" << configId << m_config->settings();
        
        QSettings * s = m_config->settings();
        s->beginGroup(configId);
        QString libName(s->value("plugin", "").toString());
        // TODO/FIXME: now it's readable. Move it to QVariant in settings later with GUI
        //QPointF position(s->value("position", QPointF(10, 10)).toPointF());
        //QSizeF size(s->value("size", QSizeF(100, 100)).toSizeF());
        qreal x = s->value("x", 10.0).toReal();
        qreal y = s->value("y", 10.0).toReal();
        qreal w = s->value("w", 10.0).toReal();
        qreal h = s->value("h", 10.0).toReal();
        QPointF position(x, y);
        QSizeF size(w, h);
        s->endGroup();
        
        qDebug() << libName << position;
        QString libraryFileName = QString(DESKTOP_PLUGIN_DIR) + "libdesktop-razor-" + libName + ".so";

        qDebug() << "RazorDesktop: try to load " << libraryFileName;

        QLibrary * lib = new QLibrary(libraryFileName);
        DesktopWidgetInitFunction initFunc = (DesktopWidgetInitFunction) lib->resolve("init");
        if (!initFunc)
        {
            qDebug() << lib->errorString();
            delete lib;
        }
        else
        {
            DesktopWidgetPlugin * plugin = initFunc(configId, m_config);
            Q_ASSERT(plugin);

            if (plugin)
            {
                //lib->setParent(plugin);
                qDebug() << "    * Plugin loaded.";
                qDebug() << plugin->info();

                QGraphicsItem * item = dynamic_cast<QGraphicsItem*>(plugin);
                Q_ASSERT(item);
                m_scene->addItem(item);
                plugin->setSizeAndPosition(position, size);
            }
        }
    }
}


void RazorWorkSpace::mouseMoveEvent(QMouseEvent* _ev)
{
    QGraphicsView::mouseMoveEvent(_ev);
}

void RazorWorkSpace::mousePressEvent(QMouseEvent* _ev)
{
    QGraphicsView::mousePressEvent(_ev);
}

void RazorWorkSpace::mouseReleaseEvent(QMouseEvent* _ev)
{
    emit mouseReleased(_ev);
    QGraphicsView::mouseReleaseEvent(_ev);
}

void RazorWorkSpace::wheelEvent(QWheelEvent* _ev)
{
    int numDegrees = _ev->delta() / 8;
    int numSteps = numDegrees / 15;
    emit mouseWheeled(-numSteps);
}

RazorWorkSpace::~RazorWorkSpace()
{

}
