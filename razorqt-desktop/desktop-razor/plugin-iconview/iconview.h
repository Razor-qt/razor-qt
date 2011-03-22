#ifndef ICONVIEW_H
#define ICONVIEW_H

#include <QGraphicsView>

#include <razorqt/readsettings.h>
#include <desktopwidgetplugin.h>

class IconScene;


class IconView : public DesktopWidgetPlugin, public QGraphicsView
{
public:
    IconView(QGraphicsScene * scene, const QString & configId, ReadSettings * config);
    ~IconView();

    QString info();

    void setSizeAndPosition(const QPointF & position, const QSizeF & size);

private:
    IconScene * m_scene;
};

EXPORT_RAZOR_DESKTOP_WIDGET_PLUGIN_H


#endif
